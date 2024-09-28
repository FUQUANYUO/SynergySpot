import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import FluentUI 1.0
import Qt.labs.platform 1.0
import example 1.0
import "../component"

FluContentPage{

    id:root
    title: qsTr("Network")

    NetworkCallable{
        id:callable
        onStart: {
            showLoading()
        }
        onFinish: {
            hideLoading()
        }
        onError:
            (status,errorString,result)=>{
                console.debug(status+";"+errorString+";"+result)
            }
        onCache:
            (result)=>{
                text_info.text = result
            }
        onSuccess:
            (result)=>{
                text_info.text = result
            }
    }

    Flickable{
        id:layout_flick
        width: 200
        clip: true
        anchors{
            top: parent.top
            bottom: parent.bottom
            left: parent.left
        }
        boundsBehavior: Flickable.StopAtBounds
        ScrollBar.vertical: FluScrollBar {}
        contentHeight:layout_column.height
        Column{
            spacing: 2
            id:layout_column
            width: parent.width
            FluButton{
                implicitWidth: parent.width
                implicitHeight: 36
                text: "Get"
                onClicked: {
                    text_info.text = ""
                    Network.get("https://httpbingo.org/get")
                    .addQuery("name","孙悟空")
                    .addQuery("age",500)
                    .addQuery("address","花果山水帘洞")
                    .bind(root)
                    .go(callable)
                }
            }
            FluButton{
                implicitWidth: parent.width
                implicitHeight: 36
                text: "Head"
                onClicked: {
                    text_info.text = ""
                    Network.head("https://httpbingo.org/head")
                    .addQuery("name","孙悟空")
                    .addQuery("age",500)
                    .addQuery("address","花果山水帘洞")
                    .bind(root)
                    .go(callable)
                }
            }
            FluButton{
                implicitWidth: parent.width
                implicitHeight: 36
                text: "Post Body"
                onClicked: {
                    text_info.text = ""
                    Network.postBody("https://httpbingo.org/post")
                    .setBody("花果山水帘洞美猴王齐天大圣孙悟空")
                    .bind(root)
                    .go(callable)
                }
            }
            FluButton{
                implicitWidth: parent.width
                implicitHeight: 36
                text: "Post Form"
                onClicked: {
                    text_info.text = ""
                    Network.postForm("https://httpbingo.org/post")
                    .add("name","孙悟空")
                    .add("age",500)
                    .add("address","花果山水帘洞")
                    .bind(root)
                    .go(callable)
                }
            }
            FluButton{
                implicitWidth: parent.width
                implicitHeight: 36
                text: "Post JSON"
                onClicked: {
                    text_info.text = ""
                    Network.postJson("https://httpbingo.org/post")
                    .add("name","孙悟空")
                    .add("age",500)
                    .add("address","花果山水帘洞")
                    .bind(root)
                    .go(callable)
                }
            }
            FluButton{
                implicitWidth: parent.width
                implicitHeight: 36
                text: "Post JSON Array"
                onClicked: {
                    text_info.text = ""
                    Network.postJsonArray("https://httpbingo.org/post")
                    .add("name","孙悟空")
                    .add("age",500)
                    .add("address","花果山水帘洞")
                    .bind(root)
                    .go(callable)
                }
            }
            FluButton{
                implicitWidth: parent.width
                implicitHeight: 36
                text: "Put Body"
                onClicked: {
                    text_info.text = ""
                    Network.putBody("https://httpbingo.org/put")
                    .setBody("花果山水帘洞美猴王齐天大圣孙悟空")
                    .bind(root)
                    .go(callable)
                }
            }
            FluButton{
                implicitWidth: parent.width
                implicitHeight: 36
                text: "Put Form"
                onClicked: {
                    text_info.text = ""
                    Network.putForm("https://httpbingo.org/put")
                    .add("name","孙悟空")
                    .add("age",500)
                    .add("address","花果山水帘洞")
                    .bind(root)
                    .go(callable)
                }
            }
            FluButton{
                implicitWidth: parent.width
                implicitHeight: 36
                text: "Put JSON"
                onClicked: {
                    text_info.text = ""
                    Network.putJson("https://httpbingo.org/put")
                    .add("name","孙悟空")
                    .add("age",500)
                    .add("address","花果山水帘洞")
                    .bind(root)
                    .go(callable)
                }
            }
            FluButton{
                implicitWidth: parent.width
                implicitHeight: 36
                text: "Put JSON Array"
                onClicked: {
                    text_info.text = ""
                    Network.putJsonArray("https://httpbingo.org/put")
                    .add("name","孙悟空")
                    .add("age",500)
                    .add("address","花果山水帘洞")
                    .bind(root)
                    .go(callable)
                }
            }
            FluButton{
                implicitWidth: parent.width
                implicitHeight: 36
                text: "Patch Body"
                onClicked: {
                    text_info.text = ""
                    Network.patchBody("https://httpbingo.org/patch")
                    .setBody("花果山水帘洞美猴王齐天大圣孙悟空")
                    .bind(root)
                    .go(callable)
                }
            }
            FluButton{
                implicitWidth: parent.width
                implicitHeight: 36
                text: "Patch Form"
                onClicked: {
                    text_info.text = ""
                    Network.patchForm("https://httpbingo.org/patch")
                    .add("name","孙悟空")
                    .add("age",500)
                    .add("address","花果山水帘洞")
                    .bind(root)
                    .go(callable)
                }
            }
            FluButton{
                implicitWidth: parent.width
                implicitHeight: 36
                text: "Patch JSON"
                onClicked: {
                    text_info.text = ""
                    Network.patchJson("https://httpbingo.org/patch")
                    .add("name","孙悟空")
                    .add("age",500)
                    .add("address","花果山水帘洞")
                    .bind(root)
                    .go(callable)
                }
            }
            FluButton{
                implicitWidth: parent.width
                implicitHeight: 36
                text: "Patch JSON Array"
                onClicked: {
                    text_info.text = ""
                    Network.patchJsonArray("https://httpbingo.org/patch")
                    .add("name","孙悟空")
                    .add("age",500)
                    .add("address","花果山水帘洞")
                    .bind(root)
                    .go(callable)
                }
            }
            FluButton{
                implicitWidth: parent.width
                implicitHeight: 36
                text: "Delete Body"
                onClicked: {
                    text_info.text = ""
                    Network.deleteBody("https://httpbingo.org/delete")
                    .setBody("花果山水帘洞美猴王齐天大圣孙悟空")
                    .bind(root)
                    .go(callable)
                }
            }
            FluButton{
                implicitWidth: parent.width
                implicitHeight: 36
                text: "Delete Form"
                onClicked: {
                    text_info.text = ""
                    Network.deleteForm("https://httpbingo.org/delete")
                    .add("name","孙悟空")
                    .add("age",500)
                    .add("address","花果山水帘洞")
                    .bind(root)
                    .go(callable)
                }
            }
            FluButton{
                implicitWidth: parent.width
                implicitHeight: 36
                text: "Delete JSON"
                onClicked: {
                    text_info.text = ""
                    Network.deleteJson("https://httpbingo.org/delete")
                    .add("name","孙悟空")
                    .add("age",500)
                    .add("address","花果山水帘洞")
                    .bind(root)
                    .go(callable)
                }
            }
            FluButton{
                implicitWidth: parent.width
                implicitHeight: 36
                text: "Delete JSON Array"
                onClicked: {
                    text_info.text = ""
                    Network.deleteJsonArray("https://httpbingo.org/delete")
                    .add("name","孙悟空")
                    .add("age",500)
                    .add("address","花果山水帘洞")
                    .bind(root)
                    .go(callable)
                }
            }
            FluButton{
                implicitWidth: parent.width
                implicitHeight: 36
                text: "Open Log"
                onClicked: {
                    text_info.text = ""
                    Network.postJson("https://httpbingo.org/post")
                    .add("name","孙悟空")
                    .add("age",500)
                    .add("address","花果山水帘洞")
                    .openLog(true)
                    .bind(root)
                    .go(callable)
                }
            }
            FluButton{
                implicitWidth: parent.width
                implicitHeight: 36
                text: "Custom Header"
                onClicked: {
                    text_info.text = ""
                    Network.postJson("https://httpbingo.org/post")
                    .addHeader("os","PC")
                    .addHeader("version","1.0.0")
                    .add("name","孙悟空")
                    .add("age",500)
                    .add("address","花果山水帘洞")
                    .bind(root)
                    .go(callable)
                }
            }
            FluButton{
                implicitWidth: parent.width
                implicitHeight: 36
                text: "RequestFailedReadCache"
                onClicked: {
                    text_info.text = ""
                    Network.postJson("https://httpbingo.org/post")
                    .setCacheMode(NetworkType.RequestFailedReadCache)
                    .add("name","孙悟空")
                    .add("age",500)
                    .add("address","花果山水帘洞")
                    .add("cacheMode","RequestFailedReadCache")
                    .bind(root)
                    .go(callable)
                }
            }
            FluButton{
                implicitWidth: parent.width
                implicitHeight: 36
                text: "IfNoneCacheRequest"
                onClicked: {
                    text_info.text = ""
                    Network.postJson("https://httpbingo.org/post")
                    .setCacheMode(NetworkType.IfNoneCacheRequest)
                    .add("name","孙悟空")
                    .add("age",500)
                    .add("address","花果山水帘洞")
                    .add("cacheMode","IfNoneCacheRequest")
                    .bind(root)
                    .go(callable)
                }
            }
            FluButton{
                implicitWidth: parent.width
                implicitHeight: 36
                text: "FirstCacheThenRequest"
                onClicked: {
                    text_info.text = ""
                    Network.postJson("https://httpbingo.org/post")
                    .setCacheMode(NetworkType.FirstCacheThenRequest)
                    .add("name","孙悟空")
                    .add("age",500)
                    .add("address","花果山水帘洞")
                    .add("cacheMode","FirstCacheThenRequest")
                    .bind(root)
                    .go(callable)
                }
            }
            FluButton{
                implicitWidth: parent.width
                implicitHeight: 36
                text: "Timeout And Retry"
                onClicked: {
                    text_info.text = ""
                    Network.postJson("https://httpbingo.org/post")
                    .setTimeout(5000)
                    .setRetry(3)
                    .add("name","孙悟空")
                    .add("age",500)
                    .add("address","花果山水帘洞")
                    .add("timeout","5000")
                    .add("retry","3")
                    .bind(root)
                    .go(callable)
                }
            }
            FluProgressButton{
                id:btn_upload
                implicitWidth: parent.width
                implicitHeight: 36
                text: "Upload File"
                onClicked: {
                    file_dialog.open()
                }
            }
            FluProgressButton{
                id:btn_download
                implicitWidth: parent.width
                implicitHeight: 36
                text: "Download File"
                onClicked: {
                    folder_dialog.showDialog(function(path){
                        Network.get("http://vjs.zencdn.net/v/oceans.mp4")
                        .toDownload(path)
                        .bind(root)
                        .go(callable_download_file)
                    })
                }
            }
            FluProgressButton{
                id:btn_download_breakpoint
                implicitWidth: parent.width
                implicitHeight: 36
                text: "Breakpoint Download File"
                onClicked: {
                    folder_dialog.showDialog(function(path){
                        Network.get("http://vjs.zencdn.net/v/oceans.mp4")
                        .toDownload(path,true)
                        .bind(root)
                        .go(callable_breakpoint_download_file)
                    })
                }
            }
        }
    }

    NetworkCallable{
        id:callable_upload_file
        onStart: {
            btn_upload.disabled = true
        }
        onFinish: {
            btn_upload.disabled = false
        }
        onError:
            (status,errorString,result)=>{
                btn_upload.progress = 0
                text_info.text = result
                console.debug(status+";"+errorString+";"+result)
            }
        onSuccess:
            (result)=>{
                text_info.text = result
            }
        onUploadProgress:
            (sent,total)=>{
                btn_upload.progress = sent/total
            }
    }

    NetworkCallable{
        id:callable_download_file
        onStart: {
            btn_download.progress = 0
            btn_download.disabled = true
        }
        onFinish: {
            btn_download.disabled = false
        }
        onError:
            (status,errorString,result)=>{
                btn_download.progress = 0
                showError(errorString)
                console.debug(status+";"+errorString+";"+result)
            }
        onSuccess:
            (result)=>{
                showSuccess(result)
            }
        onDownloadProgress:
            (recv,total)=>{
                btn_download.progress = recv/total
            }
    }

    NetworkCallable{
        id:callable_breakpoint_download_file
        onStart: {
            btn_download_breakpoint.progress = 0
            btn_download_breakpoint.disabled = true
        }
        onFinish: {
            btn_download_breakpoint.disabled = false
        }
        onError:
            (status,errorString,result)=>{
                btn_download_breakpoint.progress = 0
                showError(errorString)
                console.debug(status+";"+errorString+";"+result)
            }
        onSuccess:
            (result)=>{
                showSuccess(result)
            }
        onDownloadProgress:
            (recv,total)=>{
                btn_download_breakpoint.progress = recv/total
            }
    }

    FileDialog {
        id: file_dialog
        onAccepted: {
            Network.postForm("https://httpbingo.org/post")
            .setRetry(1)//只请求一次
            .add("accessToken","12345678")
            .addFile("file",FluTools.toLocalPath(file_dialog.currentFile))
            .bind(root)
            .go(callable_upload_file)
        }
    }

    FileDialog {
        property var onSelectListener
        id: folder_dialog
        folder: StandardPaths.standardLocations(StandardPaths.DownloadLocation)[0]
        currentFile: StandardPaths.standardLocations(StandardPaths.DownloadLocation)[0]+"/oceans.mp4"
        fileMode: FileDialog.SaveFile
        onAccepted: {
            folder_dialog.onSelectListener(FluTools.toLocalPath(folder_dialog.currentFile))
        }
        function showDialog(listener){
            folder_dialog.onSelectListener = listener
            folder_dialog.open()
        }
    }

    FluFrame{
        anchors{
            top: layout_flick.top
            bottom: layout_flick.bottom
            left: layout_flick.right
            right: parent.right
            leftMargin: 8
        }
        Flickable{
            clip: true
            id:scrollview
            boundsBehavior:Flickable.StopAtBounds
            width: parent.width
            height: parent.height
            contentWidth: width
            contentHeight: text_info.height
            ScrollBar.vertical: FluScrollBar {}
            FluText{
                id:text_info
                width: scrollview.width
                wrapMode: Text.WrapAnywhere
                padding: 14
            }
        }
    }
}
