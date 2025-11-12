# Zeitgeist format
There are three types of messages: **requests** sent by the client to
trigger an action on the server, the **responses** which is the answer
the server sends in response to a request, and **server messages**
which is data the server can send to the client without depending on a
client request.

Responses, requests, and server messages formats are based on the
[HTTP](https://developer.mozilla.org/en-US/docs/Web/HTTP) format with
some differences on the **start line**.

**Components of a Zeitgeist payload**:
| Component | Purpose |
|:--|:--|
| Start line | Single line describing the message, depends on the type of message |
| Headers | Optional metadata of the message |
| Empty line | A single empty line indicating the metadata of the message is complete |
| Body | Optional data associated to the message|

Every line break is noted as `\r\n` and there is an empty line
separating the **start line** and **headers** from the **body**

**Start line**:
The only component that varies in format depending on the type of
message. Reference: the words among curly braces are set by the
sender.
| Request | Response | Server message |
|:--|:--|:--|
| `{METHOD} {TARGET} {REQ_ID}` | `ZEIT/RES {STATUS} REQ_ID` | `ZEIT/MSG` |


## Examples
**Request**:
```
POST /api/login 5555
Host: example.com
Content-Type: application/json
Content-Length: 52

{
  "username": "johndoe",
  "password": "123456"
}
```
**Response**:
```
ZEIT/RES 200 5555
Content-Type: application/json
Content-Length: 29

{
  "token": "abc123xyz"
}
````
**Server message**:
```
ZEIT/MSG
Content-Type: application/json
Content-Length: 45

{
  "message": "you have 2 unread messages"
}
```
