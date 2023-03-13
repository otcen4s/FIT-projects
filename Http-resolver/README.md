# Project Title

This is a simple http resolver project that simulates server-client communication.

## More detailed description

In this project is implemented only server side performance that processes two http methods
(e.g. GET/POST) and the client side http requests are performed by web client tools
(for example: web browser, 'curl' terminal tool).
Server side implementation uses one object named 'Server' disposing basic methods
for establishing communication (e.g. creating socket end point, binding socket to
to device ip address on the given port number, starting to listen, accepting the request, 
receiving data and decoding them).
Server is implemented like iterative server which means that only one http
request can be processed at the time and if there is more requests at the moment, they will
have to wait in a queue (which is explicitly set to 10) until the previous requests are
finished.
Maximum data size to be received is explicitly set to 2048 bytes (optional size).

### Method GET

When the client sends the http request of the following syntax : 
'GET /resolve?name="name"&type="type" HTTP/1.1' where 'type' can stand for A(ip address) 
or PTR(domain name) and 'name' can stand for domain name or ip address, the server will 
send the http respond in the following syntax:
'"domain_name":A=xxx.xxx.xxx.xxx' or 'xxx.xxx.xxx.xxx:PTR="domain_name"'.

### Method POST

When the client sends the http request of the following syntax: 
'POST /dns-query HTTP/1.1' and the body will contain queries of the following syntax :
'name:type' where name can stand for domain name or ip address and type can stand for
A(ip address) or PTR(domain name), the server http response will be :
'name:type="response"' where response can stand for either domain name or ip address.

#### Usage

This is mainly console based program which should be primary tested in terminal.
You can run this program by following steps :
1. make run (implicitly the port number is set to 1234 or use explicitly ---> make run PORT = 'number')
--- for GET method in other terminal window type ---
2. curl localhost:1234/resolve?name='name'\&type='type'
--- for POST method in other terminal window type ---
3. curl --data-binary @queries.txt -X POST http://localhost:1234/dns-query
--- to end running server press Ctrl-C ---