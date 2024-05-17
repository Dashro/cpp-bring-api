#pragma once

#include <QObject>
#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkRequest>



class QBringApi : public QObject {
    Q_OBJECT;

public:
    QBringApi(QNetworkAccessManager* nm, QObject* parent);

    void login(const QString& mail, const QString& password);

    void loadLists();
    const QMap<QString, QString>& lists() const;

    void safeItem(const QString& listUuid, const QString& item, const QString& specification = QString());

signals:
    void loggedIn();
    void listsLoaded();

private:
    QNetworkAccessManager* _nm;

    const QString url = "https://api.getbring.com/rest/v2";
    const QString apiKey = "cof4Nc6D8saplXjE3h3HXqHH8m7VU2i1Gs0g85Sp";

    QString _uuid;
    QString _publicUuid;
    QString _accessToken;

    QMap<QString, QString> _lists;

    void setHeaders(QNetworkRequest& request);
    void setPutHeaders(QNetworkRequest& request);
    void setPostHeaders(QNetworkRequest& request);
};
