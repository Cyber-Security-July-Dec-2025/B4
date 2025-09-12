#pragma once

#include <QMainWindow>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QJsonObject>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void onConnectClicked();
    void onSendClicked();
    void onDisconnectClicked();
    void onNewIncomingConnection();
    void onSocketReadyRead();
    void onSocketConnected();
    void onSocketDisconnected();

private:
    void loadConfig();
    void appendMessage(const QString& who, const QString& text);
    void processIncomingBuffer();
    void loadHistory();                // NEW: load chat.log on startup
    void persistMessage(const QString& who, const QString& text); // NEW: write to chat.log

    QTextEdit* m_display;
    QTextEdit* m_input;
    QPushButton* m_connectBtn;
    QPushButton* m_sendBtn;
    QPushButton* m_disconnectBtn;
    QLabel* m_statusLabel;

    QTcpServer* m_server;
    QTcpSocket* m_activeSocket;
    QByteArray m_recvBuffer;
    QJsonObject m_config;
    QString m_historyPath; // path to chat.log
};
