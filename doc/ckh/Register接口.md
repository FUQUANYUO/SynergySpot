#### 发送验证码

```cpp
bool sendVerifitionCode(QString emailAddress);
// 解释:
// 调用sendVerifitionCode后，检查邮箱格式，若错误，返回false。connect接收验证码的信号和处理sigTransVerifyData传入的数据的lambda表达式，然后创建EmailVerify对象并调用函数sendEmailVerifyCode发送验证码，返回true。
```

#### 信号-收到验证码

```cpp
void sigTransVerifyData(std::array<std::string,3> data);
// 解释:
// 由BusinessListen的GET_EMAILCODE触发，通过EmailVerify处理后分别传入：起始时间、有效时间、验证码
```

#### 注册账号

```cpp
QString AccountRegister(QString ssName,QString ssPassword,QString mailBox);
//解释：
//传入注册账号密码与邮箱信息，并返回账号
```

#### 