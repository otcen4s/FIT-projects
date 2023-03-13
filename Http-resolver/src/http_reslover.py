#!/usr/bin/python3

from __future__ import print_function

import socket
import sys
from re import split, search, match, sub


class Server:
    serverSocket = ""

    def __init__(self, port):
        self.port = port

    def createSocket(self):
        try:
            self.serverSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # Socket creation
            self.serverSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)  # Same address can be reused in every run
            eprint("Socket successfully created ...")

        except socket.error as err:  # Basic error handle
            eprint("Socket creation failed with error %s." % (err))
            sys.exit()

    def bindSocket(self):
        #try:
          #  host = socket.gethostbyname(socket.gethostname())  # Obtaining host name

        #except socket.gaierror:  # Basic error handle
         #   eprint("There was an error resolving the host.")
         #   sys.exit()

        try:
            self.serverSocket.bind(("", self.port))  # Server binding
            eprint("Socket successfully bind ...")

        except socket.error as err:  # Basic error handle
            eprint("Socket binding failed with error %s." % (err))
            sys.exit()

    def listenSocket(self):
        try:
            self.serverSocket.listen(10)  # Server listening started
            eprint("Server listening ...")  # Specifying queue number to max 10 requests

        except socket.error as err:  # Basic error handle
            eprint("Server listening failed with error %s." % (err))

    def decodeRequest(self):
        (clientSocket, clientAddress) = self.serverSocket.accept()  # Server is ready to accept client request
        dataIn = clientSocket.recv(2048).decode()  # Max 2048 B of data can be received and stored

        dataInPost = dataIn

        # These are string operations to get what we need from string received
        dataInGet = dataIn.split("\n")
        dataInGet[0] = dataInGet[0].strip()
        methode = dataInGet[0].split(" ")

        if methode[0] == "GET":
             dataOut = self.resolveGet(dataInGet)

        elif methode[0] == "POST":
            dataOut = self.resolvePost(dataInPost)

        else:
            dataOut = self.methodeNotAllowed()

        return (clientSocket, dataOut)  # Returning tuple

    def resolvePost(self, dataIn):
        dataInPost = self.getBody(dataIn)
        dataInPost = dataInPost.split("\n")[:-1]
        dataIn = dataIn.split("\n")
        dataIn[0] = dataIn[0].strip()
        dataOut = ""

        if dataIn[0] != "POST /dns-query HTTP/1.1":
            dataOut = self.badRequest()
            return dataOut

        for data in dataInPost:
            if data != "":
                data = split(":", data)
                
                if match(r"^(\s)*(\S)+(\s)*$", data[0]) != None and match(r"^(\s)*(PTR|A)(\s)*$", data[1]) != None:
                    data[0] = sub("\s", "", data[0])
                    data[1] = sub("\s", "", data[1])

                else:
                    continue

                if self.getType(data, "POST"):
                    try:
                        socket.inet_aton(data[0])  # Legal
                        eprint("Invalid request")
                        continue

                    except socket.error as err:  # Not legal

                        try:
                            ipAddr = socket.gethostbyname(data[0])
                            dataOut += data[0] + ":A=" + ipAddr + "\r\n"

                        except (socket.herror, socket.gaierror) as err:
                            eprint("Server obtaining IP address failed with error %s." % (err))
                else:
                    try:
                        socket.inet_aton(data[0])  # Legal

                    except socket.error as err:  # Not legal
                        eprint("Invalid request %s." % (err))
                        continue

                    try:
                        hostName = socket.gethostbyaddr(data[0])  # still causing error in same cases
                        dataOut += data[0] + ":PTR=" + hostName[0] + "\r\n"

                    except (socket.herror, socket.gaierror) as err:  # Can be also bad request or not found ...
                        eprint("Server obtaining host name failed with error %s." % (err))
            else:
                dataOut = self.badRequest()
                return dataOut


        if dataOut == "":
            dataOut = self.notFound()
            return dataOut

        contentLengthString = dataOut
        contentLength = sys.getsizeof(contentLengthString)
        dataOut = "HTTP/1.1 200 OK\r\n" + "Content-Type: text/plain; charset=utf-8\r\n" + "Content-Length: " + str(contentLength) + "\r\n" + "\r\n" + str(contentLengthString) + "\r\n\r\n"

        return dataOut

    def resolveGet(self, dataIn):
        if match(r"^((GET /resolve\?name=)(\S)+(&type=(A|PTR))( HTTP/1\.1))$", dataIn[0]) == None:
            eprint("Error: Wrong syntax")
            dataOut = self.badRequest()

        else:
            query = dataIn[0].split("GET /resolve?name=")
            query = query[1].split("&")

            if self.getType(dataIn, "GET"):  # Finding IP address
                try:
                    socket.inet_aton(query[0])  # Legal
                    eprint("Invalid request")
                    dataOut = self.badRequest()
                    return dataOut

                except socket.error as err:  # Not legal

                    try:
                        ipAddr = socket.gethostbyname(query[0])
                        dataOut = self.okFound(query[0], ipAddr, ":A=")

                    except (socket.herror, socket.gaierror) as err:
                        eprint("Server obtaining IP address failed with error %s." % (err))
                        dataOut = self.notFound()

            else:  # Finding host name of IP address given
                try:
                    socket.inet_aton(query[0])  # Legal

                except socket.error as err:  # Not legal
                    eprint("Invalid request %s." % (err))
                    dataOut = self.badRequest()
                    return dataOut

                try:
                    hostName = socket.gethostbyaddr(query[0])  # still causing error in same cases
                    dataOut = self.okFound(query[0], hostName[0], ":PTR=")

                except (socket.herror, socket.gaierror) as err:  # Can be also bad request or not found ...
                    eprint("Server obtaing host name failed with error %s." % (err))
                    dataOut = self.notFound()

        return dataOut

    def getType(self, dataIn, methodeType):  # Resolving A or PTR request
        if methodeType == "GET":
            if search("&type=A", dataIn[0]):
                return True

            elif search("&type=PTR", dataIn[0]):
                return False

        else:
            if dataIn[1] == "A":
                return True

            elif dataIn[1] == "PTR":
                return False


    def getBody(self, dataIn):
        idx = str(dataIn).find("\r\n\r\n")

        if idx >= 0:
            return dataIn[idx + 4:]
        return dataIn

    def badRequest(self):
        contentLengthString = "400 Bad Request"
        contentLength = sys.getsizeof(contentLengthString)
        dataOut = "HTTP/1.1 400 Bad Request\r\n" +  "Content-Type: text/plain; charset=utf-8\r\n" + "Content-Length: " + str(contentLength) + "\r\n" + "\r\n" + str(contentLengthString) + "\r\n\r\n"
        return dataOut

    def notFound(self):
        contentLengthString = "404 Not Found"
        contentLength = sys.getsizeof(contentLengthString)
        dataOut = "HTTP/1.1 404 Not Found\r\n" +  "Content-Type: text/plain; charset=utf-8\r\n" + "Content-Length: " + str(contentLength) + "\r\n" + "\r\n" + str(contentLengthString) + "\r\n\r\n"
        return dataOut

    def okFound(self, query, ipOrHost, ptrOrAddr):
        contentLengthString = query + ptrOrAddr + ipOrHost
        contentLength = sys.getsizeof(contentLengthString)
        dataOut = "HTTP/1.1 200 OK\r\n" + "Content-Type: text/plain; charset=utf-8\r\n" + "Content-Length: " + str(contentLength) + "\r\n" + "\r\n" + str(contentLengthString) + "\r\n\r\n"
        return dataOut

    def internalServerError(self):
        contentLengthString = "500 Internal Server Error"
        contentLength = sys.getsizeof(contentLengthString)
        dataOut = "HTTP/1.1 500 Internal Server Error\r\n" + "Content-Type: text/plain; charset=utf-8\r\n" + "Content-Length: " + str(contentLength) + "\r\n" + "\r\n" + str(contentLengthString) + "\r\n\r\n"
        return dataOut

    def methodeNotAllowed(self):
        contentLengthString = "405 Method Not Allowed"
        contentLength = sys.getsizeof(contentLengthString)
        dataOut = "HTTP/1.1 405 Method Not Allowed\r\n" +  "Content-Type: text/plain; charset=utf-8\r\n" + "Content-Length: " + str(contentLength) + "\r\n" + "\r\n" + str(contentLengthString) + "\r\n\r\n"
        return dataOut

    def sendResponse(self, dataOut, clientSocket):
        clientSocket.sendall(dataOut.encode())  # Sending encoded data to client from server
        clientSocket.close()  # Closing communication

    def runServer(self):  # Method that calls all previous methods of server and starts communication if successful
        self.createSocket()
        self.bindSocket()
        self.listenSocket()

        try:
            while True:
                (clientSocket, dataOut) = self.decodeRequest()
                self.sendResponse(dataOut, clientSocket)

        except KeyboardInterrupt:
            eprint("\nServer shut down.")
            self.serverSocket.close()


def eprint(*args, **kwargs):  # Error handle print to stderr
    print(*args, file=sys.stderr, **kwargs)


def parseArgument():  # Simple argument parsing function
    arg = sys.argv[1]
    argSplit = split('=', arg)
    return argSplit[1]


PORT = parseArgument()  # Obtaining port number

if 65535 >= int(PORT) > 0:

    try:
        int(PORT)
    
    except ValueError:
        eprint("Port is not a number.")
        exit(1)
    
    server = Server(int(PORT))
    server.runServer()
    
else:
    eprint("Value of port number is not uint16 [0 to 65535].")
    exit(1)
