#include "RemoteConsole.h"
#include "QConsole.h"
#include <QtNetwork>

RemoteConsole::RemoteConsole()
{
}

RemoteConsole::RemoteConsole(QString _Address, int _Port)
{
    Connect(_Address, _Port);
}

int RemoteConsole::Connect(QString address, int port)
{
    sAddress = address;
    iPort = port;
    LOG("[NET] Connecting to server socket...");
    if (iState == 0)  //Disconnected
    {
        socket.connectToHost(address, port);

        if (!socket.waitForConnected(SEC_TIMEOUT))
        {
            QConsole() << "Impossible reach host!\n";
            LOG("[NET] Connection timeout!");
            iState = 2; //Timeout
            return -1;
        }
        LOG("[NET] Connected to server!");
        iState = 1; //Connected
        return 0;
    }
    LOG("[NET] Already connected!");
    return -1;
}

int RemoteConsole::Login(QString passwd)
{
    LOG("[NET] Trying to login...");
    QString data;
    do
    {
        data = ReadStream();
    }while(data != "login: \r");
    ReadStream();
    ReadStream();
    ReadStream();
    WriteStream(passwd); //gli invio la password
    ReadStream();
    data = ReadStream();//ottengo il risultato della connessione:
    if (data == "login: \r")// se mi richiede il login: vuol dire che è sbagliata la password
    {
        LOG("[NET] Login failed!");
        QConsole() << "\nWrong Password!\n";
        return -1;
    }
    LOG("[NET] Connection authenticated!");
    return 0;
}

QString RemoteConsole::ReadStream()
{
    char buffer;
    QString strout;
    socket.waitForReadyRead(100);
    while(socket.bytesAvailable() != 0 && buffer != '\r')
    {
    socket.read(&buffer, 1);
    if (buffer >= 0 && buffer <= 127)
        strout.append(buffer);
    }
    return strout;
}

int RemoteConsole::WriteStream(QString send)
{
    send.append("\r");
    socket.write( send.toUtf8() );
    socket.waitForBytesWritten();
    return 0;
}

void RemoteConsole::Close()
{
     LOG("[NET] Closing socket!");
    iState = 0;
    socket.close();
}

bool RemoteConsole::isOnline()
{
    return iState == 1;
}

QString RemoteConsole::getAddress() { return sAddress; }
QString RemoteConsole::getPort() { return QString::number(iPort); }
