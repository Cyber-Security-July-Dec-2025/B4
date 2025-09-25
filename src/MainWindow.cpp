#include "MainWindow.h"
#include "CryptoHelper.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QJsonDocument>
#include <QDateTime>
#include <QHostAddress>
#include <QMessageBox>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent),
    m_server(new QTcpServer(this)),
    m_activeSocket(nullptr)
{
    QWidget* w = new QWidget;
    QVBoxLayout* mainL = new QVBoxLayout;
    m_display = new QTextEdit; m_display->setReadOnly(true);
    m_input = new QTextEdit; m_input->setFixedHeight(90);
    m_connectBtn = new QPushButton("Connect");
    m_sendBtn = new QPushButton("Send");
    m_disconnectBtn = new QPushButton("Disconnect");
    m_statusLabel = new QLabel("Not connected");

    QHBoxLayout* hb = new QHBoxLayout;
    hb->addWidget(m_connectBtn);
    hb->addWidget(m_disconnectBtn);
    hb->addWidget(m_sendBtn);
    hb->addStretch();
    hb->addWidget(m_statusLabel);

    mainL->addWidget(m_display);
    // --- separator line above the input area ---
    QFrame* sep = new QFrame;
    sep->setObjectName("inputSeparator");
    // use a horizontal line but we'll style it with QSS, so set no frame style
    sep->setFrameShape(QFrame::HLine);
    sep->setFrameShadow(QFrame::Plain);
    sep->setFixedHeight(1);
    sep->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    mainL->addWidget(sep);
    // --- end separator ---

    mainL->addWidget(m_input);
    mainL->addLayout(hb);
    w->setLayout(mainL);
    setCentralWidget(w);
    setWindowTitle("SafeTalk (Qt + Crypto++)");

    connect(m_connectBtn, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    connect(m_sendBtn, &QPushButton::clicked, this, &MainWindow::onSendClicked);
    connect(m_disconnectBtn, &QPushButton::clicked, this, &MainWindow::onDisconnectClicked);
    connect(m_server, &QTcpServer::newConnection, this, &MainWindow::onNewIncomingConnection);

    loadConfig();
    m_historyPath = QDir::current().filePath("chat.log");
    loadHistory();
    QString listenIp = m_config.value("listen_ip").toString("0.0.0.0");
    int listenPort = m_config.value("listen_port").toInt(9001);
    QHostAddress host(listenIp);
    if (!m_server->listen(host, listenPort)) {
        QMessageBox::warning(this, "Server", QString("Failed to listen on %1:%2").arg(listenIp).arg(listenPort));
    } else {
        appendMessage("System", QString("Listening on %1:%2").arg(listenIp).arg(listenPort));
    }
}

void MainWindow::loadConfig() {
    QFile f("config.json");
    if (!f.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, "Config", "Failed to open config.json in current directory");
        return;
    }
    QByteArray b = f.readAll();
    QJsonDocument d = QJsonDocument::fromJson(b);
    m_config = d.object();
}

void MainWindow::onNewIncomingConnection() {
    QTcpSocket* clientSock = m_server->nextPendingConnection();
    if (m_activeSocket) {
        m_activeSocket->disconnectFromHost();
    }
    m_activeSocket = clientSock;
    connect(m_activeSocket, &QTcpSocket::readyRead, this, &MainWindow::onSocketReadyRead);
    connect(m_activeSocket, &QTcpSocket::connected, this, &MainWindow::onSocketConnected);
    connect(m_activeSocket, &QTcpSocket::disconnected, this, &MainWindow::onSocketDisconnected);
    appendMessage("System", "Accepted incoming connection");
    m_statusLabel->setText("Connected (incoming)");
}

void MainWindow::onConnectClicked() {
    QString peerIp = m_config.value("peer_ip").toString();
    int peerPort = m_config.value("peer_port").toInt();
    if (m_activeSocket) {
        appendMessage("System", "Already connected. Disconnect first.");
        return;
    }
    m_activeSocket = new QTcpSocket(this);
    connect(m_activeSocket, &QTcpSocket::readyRead, this, &MainWindow::onSocketReadyRead);
    connect(m_activeSocket, &QTcpSocket::connected, this, &MainWindow::onSocketConnected);
    connect(m_activeSocket, &QTcpSocket::disconnected, this, &MainWindow::onSocketDisconnected);

    m_activeSocket->connectToHost(peerIp, peerPort);
    appendMessage("System", QString("Connecting to %1:%2 ...").arg(peerIp).arg(peerPort));
}

void MainWindow::onSocketConnected() {
    m_statusLabel->setText("Connected");
    appendMessage("System", "Socket connected");
}

void MainWindow::onSocketDisconnected() {
    m_statusLabel->setText("Disconnected");
    appendMessage("System", "Socket disconnected");
    if (m_activeSocket) {
        m_activeSocket->deleteLater();
        m_activeSocket = nullptr;
    }
}

void MainWindow::loadHistory() {
    QFile f(m_historyPath);
    if (!f.exists()) return;
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream in(&f);
    while (!in.atEnd()) {
        QString line = in.readLine();
        // history lines are formatted as: [YYYY-MM-DD hh:mm:ss] <WHO>: message
        // We'll append as raw HTML to preserve what was logged.
        m_display->append(line.toHtmlEscaped());
    }
    m_display->moveCursor(QTextCursor::End);
    f.close();
}

void MainWindow::persistMessage(const QString& who, const QString& text) {
    QFile f(m_historyPath);
    if (!f.open(QIODevice::Append | QIODevice::Text)) return;
    QTextStream out(&f);
    QString ts = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    // Save the same HTML we display (so reload preserves look)
    QString html = QString("[%1] <b>%2</b>: %3").arg(ts, who, text.toHtmlEscaped());
    out << html << "\n";
    f.close();
}


void MainWindow::onDisconnectClicked() {
    if (m_activeSocket) m_activeSocket->disconnectFromHost();
}

void MainWindow::onSendClicked() {
    if (!m_activeSocket || m_activeSocket->state() != QAbstractSocket::ConnectedState) {
        appendMessage("System", "Not connected to peer.");
        return;
    }
    QString plaintext = m_input->toPlainText().trimmed();
    if (plaintext.isEmpty()) return;

    std::string peerPub = m_config.value("peer_public_key").toString().toStdString();
    std::string envelope;
    try {
        envelope = CryptoHelper::EncryptForPeer(plaintext.toStdString(), peerPub);
    } catch (const std::exception &ex) {
        appendMessage("System", QString("Encryption error: %1").arg(ex.what()));
        return;
    }

    quint32 len = static_cast<quint32>(envelope.size());
    QByteArray out;
    out.resize(4);
    out[0] = char((len >> 24) & 0xFF);
    out[1] = char((len >> 16) & 0xFF);
    out[2] = char((len >> 8) & 0xFF);
    out[3] = char(len & 0xFF);
    out.append(QByteArray(envelope.data(), static_cast<int>(envelope.size())));

    qint64 written = m_activeSocket->write(out);
    m_activeSocket->flush();
    (void)written;

    appendMessage("Me", plaintext);
    m_input->clear();
}

void MainWindow::onSocketReadyRead() {
    if (!m_activeSocket) return;
    m_recvBuffer.append(m_activeSocket->readAll());
    processIncomingBuffer();
}

void MainWindow::processIncomingBuffer() {
    while (true) {
        if (m_recvBuffer.size() < 4) return;
        const unsigned char* p = reinterpret_cast<const unsigned char*>(m_recvBuffer.constData());
        quint32 msgLen = (static_cast<quint32>(p[0])<<24) | (static_cast<quint32>(p[1])<<16) | (static_cast<quint32>(p[2])<<8) | static_cast<quint32>(p[3]);
        if (static_cast<quint32>(m_recvBuffer.size()) < 4 + msgLen) return;
        QByteArray payload = m_recvBuffer.mid(4, msgLen);
        m_recvBuffer.remove(0, 4 + msgLen);

        QString myPriv = m_config.value("my_private_key").toString();
        try {
            std::string env(payload.constData(), payload.size());
            std::string text = CryptoHelper::DecryptWithMyPrivate(env, myPriv.toStdString());
            appendMessage("Peer", QString::fromStdString(text));
        } catch (const std::exception &ex) {
            appendMessage("System", QString("Decrypt error: %1").arg(ex.what()));
        }
    }
}

void MainWindow::appendMessage(const QString& who, const QString& text) {
    QString ts = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString color;
    if (who == "Me") color = "#0B63CE";          // blue
    else if (who == "Peer") color = "#0A9A6C";   // green
    else color = "#777777";                      // system: gray

    // Build HTML line with small rounded badge
    QString html = QString("<div style='margin:4px 0;'>"
                           "<span style='color:%1; font-weight:600;'>[%2]</span> "
                           "<span style='background:%3; color:white; padding:2px 6px; border-radius:8px; font-size:11px; margin-left:6px;'>%4</span> "
                           "<span style='margin-left:8px;'>%5</span>"
                           "</div>")
                        .arg("#555555", ts, color, who, text.toHtmlEscaped());

    m_display->append(html);
    m_display->moveCursor(QTextCursor::End);

    // Persist the message
    persistMessage(who, text);
}

