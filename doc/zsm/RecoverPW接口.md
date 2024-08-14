#### 发送验证码

```cpp
bool sendVerifitionCode(QString emailAddress);
// 解释:
// 检查邮箱格式，若错误，返回false。connect连接接收验证码的信号和处理sigTransVerifyData传入的数据的lambda表达式，然后创建EmailVerify对象并调用函数sendEmailVerifyCode发送验证码，返回true。
```

#### 信号-收到验证码

```cpp
void sigRecvVerifitionData(std::array<std::string,3> data);
// 解释:
// 由BusinessListen的GET_EMAILCODE触发，通过EmailVerify处理后分别传入：起始时间、有效时间、验证码
```

#### 判断账号邮箱是否匹配

```cpp
void AEIsSuited(QString emailAddress, QString account);
// 解释:
// 调用changePassword后，前端进入加载页面，调用BusinessListen（或其他进一步封装的类）的信号（还没写）向服务端发送请求判断账号邮箱是否匹配。
```

#### 信号-接收账号邮箱是否对应的结果

```cpp
void sigSuitedResult(bool ret);
// 解释：
// 由BusinessListen内收到服务端结果的信号（还没写）触发，传入是否匹配的结果
```

#### 修改密码

```cpp
void changePassword(QString account，QString newPassword);
// 解释：
// 调用其他类（还没写）向服务端发送修改密码的消息
```

#### 信号-修改密码是否成功结果

```cpp
void sigChangeIsSuccess(bool ret);
// 解释：
// 由BusinessListen内收到服务端结果的信号（还没写）触发，传入修改是否成功
```

