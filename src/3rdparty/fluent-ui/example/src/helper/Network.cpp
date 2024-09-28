#include "Network.h"

#include <QUrlQuery>
#include <QBuffer>
#include <QHttpMultiPart>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QJsonObject>
#include <QNetworkDiskCache>
#include <QQmlEngine>
#include <QQmlContext>
#include <QJSEngine>
#include <QJsonArray>
#include <QStandardPaths>
#include <QThreadPool>
#include <QDir>
#include <QCryptographicHash>
#include <QEventLoop>
#include <QGuiApplication>
#include <utility>

NetworkCallable::NetworkCallable(QObject *parent) : QObject{parent} {
}

QString NetworkParams::method2String() const {
    switch (_method) {
        case METHOD_GET:
            return "GET";
        case METHOD_HEAD:
            return "HEAD";
        case METHOD_POST:
            return "POST";
        case METHOD_PUT:
            return "PUT";
        case METHOD_PATCH:
            return "PATCH";
        case METHOD_DELETE:
            return "DELETE";
        default:
            return "";
    }
}

int NetworkParams::getTimeout() const {
    if (_timeout != -1) {
        return _timeout;
    }
    return Network::getInstance()->timeout();
}

int NetworkParams::getRetry() const {
    if (_retry != -1) {
        return _retry;
    }
    return Network::getInstance()->retry();
}

bool NetworkParams::getOpenLog() const {
    if (!_openLog.isNull()) {
        return _openLog.toBool();
    }
    return Network::getInstance()->openLog();
}

FluDownloadParam::FluDownloadParam(QObject *parent) : QObject{parent} {
}

FluDownloadParam::FluDownloadParam(QString destPath, bool append, QObject *parent)
    : QObject{parent} {
    this->_destPath = std::move(destPath);
    this->_append = append;
}

NetworkParams::NetworkParams(QObject *parent) : QObject{parent} {
    _method = NetworkParams::Method::METHOD_GET;
    _type = NetworkParams::Type::TYPE_BODY;
}

NetworkParams::NetworkParams(QString url, Type type, Method method, QObject *parent)
    : QObject{parent} {
    this->_method = method;
    this->_url = std::move(url);
    this->_type = type;
}

NetworkParams *NetworkParams::add(const QString &key, const QVariant &val) {
    _paramMap.insert(key, val);
    return this;
}

NetworkParams *NetworkParams::addFile(const QString &key, const QVariant &val) {
    _fileMap.insert(key, val);
    return this;
}

NetworkParams *NetworkParams::addHeader(const QString &key, const QVariant &val) {
    _headerMap.insert(key, val);
    return this;
}

NetworkParams *NetworkParams::addQuery(const QString &key, const QVariant &val) {
    _queryMap.insert(key, val);
    return this;
}

NetworkParams *NetworkParams::setBody(QString val) {
    _body = std::move(val);
    return this;
}

NetworkParams *NetworkParams::setTimeout(int val) {
    _timeout = val;
    return this;
}

NetworkParams *NetworkParams::setRetry(int val) {
    _retry = val;
    return this;
}

NetworkParams *NetworkParams::setCacheMode(int val) {
    _cacheMode = val;
    return this;
}

NetworkParams *NetworkParams::toDownload(QString destPath, bool append) {
    _downloadParam = new FluDownloadParam(std::move(destPath), append, this);
    return this;
}

NetworkParams *NetworkParams::bind(QObject *target) {
    _target = target;
    return this;
}

NetworkParams *NetworkParams::openLog(QVariant val) {
    _openLog = std::move(val);
    return this;
}

QString NetworkParams::buildCacheKey() const {
    QJsonObject obj;
    obj.insert("url", _url);
    obj.insert("method", method2String());
    obj.insert("body", _body);
    obj.insert("query", QJsonDocument::fromVariant(_queryMap).object());
    obj.insert("param", QJsonDocument::fromVariant(_paramMap).object());
    obj.insert("header", QJsonDocument::fromVariant(_headerMap).object());
    obj.insert("file", QJsonDocument::fromVariant(_fileMap).object());
    if (_downloadParam) {
        QJsonObject downObj;
        downObj.insert("destPath", _downloadParam->_destPath);
        downObj.insert("append", _downloadParam->_append);
        obj.insert("download", downObj);
    }
    QByteArray data = QJsonDocument(obj).toJson(QJsonDocument::Compact);
    return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
}

void NetworkParams::go(NetworkCallable *callable) {
    QJSValueList data;
    data << qjsEngine(callable)->newQObject(this);
    Network::getInstance()->_interceptor.call(data);
    if (_downloadParam) {
        Network::getInstance()->handleDownload(this, callable);
    } else {
        Network::getInstance()->handle(this, callable);
    }
}

void Network::handle(NetworkParams *params, NetworkCallable *c) {
    QPointer<NetworkCallable> callable(c);
    QThreadPool::globalInstance()->start([=]() {
        if (!callable.isNull()) {
            callable->start();
        }
        QString cacheKey = params->buildCacheKey();
        if (params->_cacheMode == NetworkType::CacheMode::FirstCacheThenRequest &&
            cacheExists(cacheKey)) {
            if (!callable.isNull()) {
                callable->cache(readCache(cacheKey));
            }
        }
        if (params->_cacheMode == NetworkType::CacheMode::IfNoneCacheRequest &&
            cacheExists(cacheKey)) {
            if (!callable.isNull()) {
                callable->cache(readCache(cacheKey));
                callable->finish();
                params->deleteLater();
            }
            return;
        }
        QNetworkAccessManager manager;
        manager.setTransferTimeout(params->getTimeout());
        QEventLoop loop;
        connect(&manager, &QNetworkAccessManager::finished, &manager,
                [&loop](QNetworkReply *reply) { loop.quit(); });
        for (int i = 0; i <= params->getRetry() - 1; ++i) {
            QUrl url(params->_url);
            addQueryParam(&url, params->_queryMap);
            QNetworkRequest request(url);
            addHeaders(&request, params->_headerMap);
            QNetworkReply *reply;
            sendRequest(&manager, request, params, reply, i == 0, callable);
            if (!QPointer<QCoreApplication>(QGuiApplication::instance())) {
                reply->deleteLater();
                reply = nullptr;
                return;
            }
            auto abortCallable = [reply, &i, params] {
                if (reply) {
                    i = params->getRetry();
                    reply->abort();
                }
            };
            QMetaObject::Connection conn_destroyed = {};
            QMetaObject::Connection conn_quit = {};
            if (params->_target) {
                conn_destroyed =
                    connect(params->_target, &QObject::destroyed, &manager, abortCallable);
            }
            conn_quit = connect(QGuiApplication::instance(), &QGuiApplication::aboutToQuit,
                                &manager, abortCallable);
            loop.exec();
            if (conn_destroyed) {
                disconnect(conn_destroyed);
            }
            if (conn_quit) {
                disconnect(conn_quit);
            }
            QString response;
            if (params->_method == NetworkParams::METHOD_HEAD) {
                response = headerList2String(reply->rawHeaderPairs());
            } else {
                if (reply->isOpen()) {
                    response = QString::fromUtf8(reply->readAll());
                }
            }
            int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            if (httpStatus == 200) {
                if (!callable.isNull()) {
                    if (params->_cacheMode != NetworkType::CacheMode::NoCache) {
                        saveResponse(cacheKey, response);
                    }
                    callable->success(response);
                }
                printRequestEndLog(request, params, reply, response);
                break;
            } else {
                if (i == params->getRetry() - 1) {
                    if (!callable.isNull()) {
                        if (params->_cacheMode == NetworkType::CacheMode::RequestFailedReadCache &&
                            cacheExists(cacheKey)) {
                            if (!callable.isNull()) {
                                callable->cache(readCache(cacheKey));
                            }
                        }
                        callable->error(httpStatus, reply->errorString(), response);
                    }
                    printRequestEndLog(request, params, reply, response);
                }
            }
            reply->deleteLater();
        }
        params->deleteLater();
        if (!callable.isNull()) {
            callable->finish();
        }
    });
}

void Network::handleDownload(NetworkParams *params, NetworkCallable *c) {
    QPointer<NetworkCallable> callable(c);
    QThreadPool::globalInstance()->start([=]() {
        if (!callable.isNull()) {
            callable->start();
        }
        QString cacheKey = params->buildCacheKey();
        QUrl url(params->_url);
        QNetworkAccessManager manager;
        manager.setTransferTimeout(params->getTimeout());
        addQueryParam(&url, params->_queryMap);
        QNetworkRequest request(url);
        addHeaders(&request, params->_headerMap);
        QString cachePath = getCacheFilePath(cacheKey);
        QString destPath = params->_downloadParam->_destPath;
        auto *destFile = new QFile(destPath);
        auto *cacheFile = new QFile(cachePath);
        bool isOpen;
        qint64 seek;
        if (cacheFile->exists() && destFile->exists() && params->_downloadParam->_append) {
            QJsonObject cacheInfo = QJsonDocument::fromJson(readCache(cacheKey).toUtf8()).object();
            qint64 fileSize = qRound(cacheInfo.value("fileSize").toDouble());
            qint64 contentLength = qRound(cacheInfo.value("contentLength").toDouble());
            if (fileSize == contentLength && destFile->size() == contentLength) {
                if (!callable.isNull()) {
                    callable->downloadProgress(fileSize, contentLength);
                    callable->success(destPath);
                    callable->finish();
                }
                return;
            }
            if (fileSize == destFile->size()) {
                request.setRawHeader("Range", QString("bytes=%1-").arg(fileSize).toUtf8());
                seek = fileSize;
                isOpen = destFile->open(QIODevice::WriteOnly | QIODevice::Append);
            } else {
                isOpen = destFile->open(QIODevice::WriteOnly | QIODevice::Truncate);
            }
        } else {
            isOpen = destFile->open(QIODevice::WriteOnly | QIODevice::Truncate);
        }
        if (!isOpen) {
            if (!callable.isNull()) {
                callable->error(-1, "device not open", "");
                callable->finish();
            }
            return;
        }
        if (params->_downloadParam->_append) {
            if (!cacheFile->open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                if (!callable.isNull()) {
                    callable->error(-1, "cache file device not open", "");
                    callable->finish();
                }
                return;
            }
        }
        QEventLoop loop;
        QNetworkReply *reply = manager.get(request);
        destFile->setParent(reply);
        cacheFile->setParent(reply);
        auto abortCallable = [reply] {
            if (reply) {
                reply->abort();
            }
        };
        connect(&manager, &QNetworkAccessManager::finished, &manager,
                [&loop](QNetworkReply *reply) { loop.quit(); });
        connect(QGuiApplication::instance(), &QGuiApplication::aboutToQuit, &manager,
                [&loop, reply]() { reply->abort(), loop.quit(); });
        QMetaObject::Connection conn_destroyed = {};
        QMetaObject::Connection conn_quit = {};
        if (params->_target) {
            conn_destroyed = connect(params->_target, &QObject::destroyed, &manager, abortCallable);
        }
        conn_quit = connect(QGuiApplication::instance(), &QGuiApplication::aboutToQuit, &manager,
                            abortCallable);
        connect(reply, &QNetworkReply::readyRead, reply,
                [reply, seek, destFile, cacheFile, callable] {
                    if (!reply || !destFile || reply->error() != QNetworkReply::NoError) {
                        return;
                    }
                    QMap<QString, QVariant> downInfo;
                    qint64 contentLength =
                        reply->header(QNetworkRequest::ContentLengthHeader).toLongLong() + seek;
                    downInfo.insert("contentLength", contentLength);
                    QString eTag = reply->header(QNetworkRequest::ETagHeader).toString();
                    downInfo.insert("eTag", eTag);
                    destFile->write(reply->readAll());
                    destFile->flush();
                    downInfo.insert("fileSize", destFile->size());
                    if (cacheFile->isOpen()) {
                        cacheFile->resize(0);
                        cacheFile->write(
                            QJsonDocument::fromVariant(QVariant(downInfo)).toJson().toBase64());
                        cacheFile->flush();
                    }
                    if (!callable.isNull()) {
                        callable->downloadProgress(destFile->size(), contentLength);
                    }
                });
        loop.exec();
        int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (httpStatus == 200) {
            if (!callable.isNull()) {
                callable->success(destPath);
            }
            printRequestEndLog(request, params, reply, destPath);
        } else {
            if (!callable.isNull()) {
                callable->error(httpStatus, reply->errorString(), destPath);
            }
            printRequestEndLog(request, params, reply, destPath);
        }
        if (conn_destroyed) {
            disconnect(conn_destroyed);
        }
        if (conn_quit) {
            disconnect(conn_quit);
        }
        params->deleteLater();
        reply->deleteLater();
        if (!callable.isNull()) {
            callable->finish();
        }
    });
}

QString Network::readCache(const QString &key) {
    auto filePath = getCacheFilePath(key);
    QString result;
    QFile file(filePath);
    if (!file.exists()) {
        return result;
    }
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream stream(&file);
        result = QString(QByteArray::fromBase64(stream.readAll().toUtf8()));
    }
    return result;
}

bool Network::cacheExists(const QString &key) {
    return QFile(getCacheFilePath(key)).exists();
}

QString Network::getCacheFilePath(const QString &key) {
    QDir cacheDir(_cacheDir);
    if (!cacheDir.exists()) {
        cacheDir.mkpath(_cacheDir);
    }
    return cacheDir.absoluteFilePath(key);
}

QString Network::headerList2String(const QList<QNetworkReply::RawHeaderPair> &data) {
    QJsonObject object;
    for (auto it = data.constBegin(); it != data.constEnd(); ++it) {
        object.insert(QString(it->first), QString(it->second));
    }
    return QJsonDocument(object).toJson(QJsonDocument::Compact);
}

QString Network::map2String(const QMap<QString, QVariant> &map) {
    QStringList parameters;
    for (auto it = map.constBegin(); it != map.constEnd(); ++it) {
        parameters << QString("%1=%2").arg(it.key(), it.value().toString());
    }
    return parameters.join(" ");
}

void Network::sendRequest(QNetworkAccessManager *manager, QNetworkRequest request,
                          NetworkParams *params, QNetworkReply *&reply, bool isFirst,
                          const QPointer<NetworkCallable> &callable) {
    QByteArray verb = params->method2String().toUtf8();
    switch (params->_type) {
        case NetworkParams::TYPE_FORM: {
            bool isFormData = !params->_fileMap.isEmpty();
            if (isFormData) {
                auto *multiPart = new QHttpMultiPart();
                multiPart->setContentType(QHttpMultiPart::FormDataType);
                for (const auto &each : params->_paramMap.toStdMap()) {
                    QHttpPart part;
                    part.setHeader(QNetworkRequest::ContentDispositionHeader,
                                   QString("form-data; name=\"%1\"").arg(each.first));
                    part.setBody(each.second.toByteArray());
                    multiPart->append(part);
                }
                for (const auto &each : params->_fileMap.toStdMap()) {
                    QString filePath = each.second.toString();
                    QString name = each.first;
                    auto *file = new QFile(filePath);
                    QString fileName = QFileInfo(filePath).fileName();
                    file->open(QIODevice::ReadOnly);
                    file->setParent(multiPart);
                    QHttpPart part;
                    part.setHeader(
                        QNetworkRequest::ContentDispositionHeader,
                        QString(R"(form-data; name="%1"; filename="%2")").arg(name, fileName));
                    part.setBodyDevice(file);
                    multiPart->append(part);
                }
                reply = manager->sendCustomRequest(request, verb, multiPart);
                multiPart->setParent(reply);
                connect(reply, &QNetworkReply::uploadProgress, reply,
                        [callable](qint64 bytesSent, qint64 bytesTotal) {
                            if (!callable.isNull() && bytesSent != 0 && bytesTotal != 0) {
                                Q_EMIT callable->uploadProgress(bytesSent, bytesTotal);
                            }
                        });
            } else {
                request.setHeader(QNetworkRequest::ContentTypeHeader,
                                  QString("application/x-www-form-urlencoded"));
                QString value;
                for (const auto &each : params->_paramMap.toStdMap()) {
                    value += QString("%1=%2").arg(each.first, each.second.toString());
                    value += "&";
                }
                if (!params->_paramMap.isEmpty()) {
                    value.chop(1);
                }
                QByteArray data = value.toUtf8();
                reply = manager->sendCustomRequest(request, verb, data);
            }
            break;
        }
        case NetworkParams::TYPE_JSON: {
            request.setHeader(QNetworkRequest::ContentTypeHeader,
                              QString("application/json;charset=utf-8"));
            QJsonObject json;
            for (const auto &each : params->_paramMap.toStdMap()) {
                json.insert(each.first, each.second.toJsonValue());
            }
            QByteArray data = QJsonDocument(json).toJson(QJsonDocument::Compact);
            reply = manager->sendCustomRequest(request, verb, data);
            break;
        }
        case NetworkParams::TYPE_JSONARRAY: {
            request.setHeader(QNetworkRequest::ContentTypeHeader,
                              QString("application/json;charset=utf-8"));
            QJsonArray jsonArray;
            for (const auto &each : params->_paramMap.toStdMap()) {
                QJsonObject json;
                json.insert(each.first, each.second.toJsonValue());
                jsonArray.append(json);
            }
            QByteArray data = QJsonDocument(jsonArray).toJson(QJsonDocument::Compact);
            reply = manager->sendCustomRequest(request, params->method2String().toUtf8(), data);
            break;
        }
        case NetworkParams::TYPE_BODY: {
            request.setHeader(QNetworkRequest::ContentTypeHeader,
                              QString("text/plain;charset=utf-8"));
            QByteArray data = params->_body.toUtf8();
            reply = manager->sendCustomRequest(request, verb, data);
            break;
        }
        default:
            reply = manager->sendCustomRequest(request, verb);
            break;
    }
    if (isFirst) {
        printRequestStartLog(request, params);
    }
}

void Network::printRequestStartLog(const QNetworkRequest &request, NetworkParams *params) {
    if (!params->getOpenLog()) {
        return;
    }
    qDebug() << "<------"
             << qUtf8Printable(request.header(QNetworkRequest::UserAgentHeader).toString())
             << "Request Start ------>";
    qDebug() << qUtf8Printable(QString::fromStdString("<%1>").arg(params->method2String()))
             << qUtf8Printable(params->_url);
    auto contentType = request.header(QNetworkRequest::ContentTypeHeader).toString();
    if (!contentType.isEmpty()) {
        qDebug() << qUtf8Printable(
            QString::fromStdString("<Header> %1=%2").arg("Content-Type", contentType));
    }
    QList<QByteArray> headers = request.rawHeaderList();
    for (const QByteArray &header : headers) {
        qDebug() << qUtf8Printable(
            QString::fromStdString("<Header> %1=%2").arg(header, request.rawHeader(header)));
    }
    if (!params->_queryMap.isEmpty()) {
        qDebug() << "<Query>" << qUtf8Printable(map2String(params->_queryMap));
    }
    if (!params->_paramMap.isEmpty()) {
        qDebug() << "<Param>" << qUtf8Printable(map2String(params->_paramMap));
    }
    if (!params->_fileMap.isEmpty()) {
        qDebug() << "<File>" << qUtf8Printable(map2String(params->_fileMap));
    }
    if (!params->_body.isEmpty()) {
        qDebug() << "<Body>" << qUtf8Printable(params->_body);
    }
}

void Network::printRequestEndLog(const QNetworkRequest &request, NetworkParams *params,
                                 QNetworkReply *&reply, const QString &response) {
    if (!params->getOpenLog()) {
        return;
    }
    qDebug() << "<------"
             << qUtf8Printable(request.header(QNetworkRequest::UserAgentHeader).toString())
             << "Request End ------>";
    qDebug() << qUtf8Printable(QString::fromStdString("<%1>").arg(params->method2String()))
             << qUtf8Printable(params->_url);
    qDebug() << "<Result>" << qUtf8Printable(response);
}

void Network::saveResponse(const QString &key, const QString &response) {
    QSharedPointer<QFile> file(new QFile(getCacheFilePath(key)));
    QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Truncate;
    if (!file->open(mode)) {
        return;
    }
    file->write(response.toUtf8().toBase64());
}

void Network::addHeaders(QNetworkRequest *request, const QMap<QString, QVariant> &headers) {
    request->setHeader(
        QNetworkRequest::UserAgentHeader,
        QString::fromStdString("Mozilla/5.0 %1/%2")
            .arg(QGuiApplication::applicationName(), QGuiApplication::applicationVersion()));
    QMapIterator<QString, QVariant> iter(headers);
    while (iter.hasNext()) {
        iter.next();
        request->setRawHeader(iter.key().toUtf8(), iter.value().toString().toUtf8());
    }
}

void Network::addQueryParam(QUrl *url, const QMap<QString, QVariant> &params) {
    QMapIterator<QString, QVariant> iter(params);
    QUrlQuery urlQuery(*url);
    while (iter.hasNext()) {
        iter.next();
        urlQuery.addQueryItem(iter.key(), iter.value().toString());
    }
    url->setQuery(urlQuery);
}

Network::Network(QObject *parent) : QObject{parent} {
    _timeout = 5000;
    _retry = 3;
    _openLog = false;
    _cacheDir = QStandardPaths::writableLocation(QStandardPaths::CacheLocation)
                    .append(QDir::separator())
                    .append("network");
}

NetworkParams *Network::get(const QString &url) {
    return new NetworkParams(url, NetworkParams::TYPE_NONE, NetworkParams::METHOD_GET, this);
}

NetworkParams *Network::head(const QString &url) {
    return new NetworkParams(url, NetworkParams::TYPE_NONE, NetworkParams::METHOD_HEAD, this);
}

NetworkParams *Network::postBody(const QString &url) {
    return new NetworkParams(url, NetworkParams::TYPE_BODY, NetworkParams::METHOD_POST, this);
}

NetworkParams *Network::putBody(const QString &url) {
    return new NetworkParams(url, NetworkParams::TYPE_BODY, NetworkParams::METHOD_PUT, this);
}

NetworkParams *Network::patchBody(const QString &url) {
    return new NetworkParams(url, NetworkParams::TYPE_BODY, NetworkParams::METHOD_PATCH, this);
}

NetworkParams *Network::deleteBody(const QString &url) {
    return new NetworkParams(url, NetworkParams::TYPE_BODY, NetworkParams::METHOD_DELETE, this);
}

NetworkParams *Network::postForm(const QString &url) {
    return new NetworkParams(url, NetworkParams::TYPE_FORM, NetworkParams::METHOD_POST, this);
}

NetworkParams *Network::putForm(const QString &url) {
    return new NetworkParams(url, NetworkParams::TYPE_FORM, NetworkParams::METHOD_PUT, this);
}

NetworkParams *Network::patchForm(const QString &url) {
    return new NetworkParams(url, NetworkParams::TYPE_FORM, NetworkParams::METHOD_PATCH, this);
}

NetworkParams *Network::deleteForm(const QString &url) {
    return new NetworkParams(url, NetworkParams::TYPE_FORM, NetworkParams::METHOD_DELETE, this);
}

NetworkParams *Network::postJson(const QString &url) {
    return new NetworkParams(url, NetworkParams::TYPE_JSON, NetworkParams::METHOD_POST, this);
}

NetworkParams *Network::putJson(const QString &url) {
    return new NetworkParams(url, NetworkParams::TYPE_JSON, NetworkParams::METHOD_PUT, this);
}

NetworkParams *Network::patchJson(const QString &url) {
    return new NetworkParams(url, NetworkParams::TYPE_JSON, NetworkParams::METHOD_PATCH, this);
}

NetworkParams *Network::deleteJson(const QString &url) {
    return new NetworkParams(url, NetworkParams::TYPE_JSON, NetworkParams::METHOD_DELETE, this);
}

NetworkParams *Network::postJsonArray(const QString &url) {
    return new NetworkParams(url, NetworkParams::TYPE_JSONARRAY, NetworkParams::METHOD_POST, this);
}

NetworkParams *Network::putJsonArray(const QString &url) {
    return new NetworkParams(url, NetworkParams::TYPE_JSONARRAY, NetworkParams::METHOD_PUT, this);
}

NetworkParams *Network::patchJsonArray(const QString &url) {
    return new NetworkParams(url, NetworkParams::TYPE_JSONARRAY, NetworkParams::METHOD_PATCH, this);
}

NetworkParams *Network::deleteJsonArray(const QString &url) {
    return new NetworkParams(url, NetworkParams::TYPE_JSONARRAY, NetworkParams::METHOD_DELETE,
                             this);
}

void Network::setInterceptor(QJSValue interceptor) {
    this->_interceptor = std::move(interceptor);
}
