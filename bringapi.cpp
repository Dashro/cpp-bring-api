#include "bringapi.h"

#include <QNetworkRequest>
#include <QNetworkReply>

#include <QUrlQuery>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


QBringApi::QBringApi(QNetworkAccessManager *nm, QObject *parent)
    : QObject(parent)
    , _nm(nm)
{
}

void QBringApi::login(const QString &mail, const QString &password)
{
    QNetworkRequest request{QUrl(url + "/bringauth")};
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QUrlQuery params;

    params.addQueryItem("email", mail);
    params.addQueryItem("password", password);

    auto* reply = _nm->post(request, params.query(QUrl::FullyEncoded).toUtf8());

    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        reply->deleteLater();

        auto data = reply->readAll();

        switch (reply->error()) {
            case QNetworkReply::NoError: {
                qInfo() << "Successfully logged in" << data.constData();

                auto json = QJsonDocument::fromJson(data).object();
                _uuid = json["uuid"].toString();
                _publicUuid = json["publicUuid"].toString();
                _accessToken = json["access_token"].toString();

                emit loggedIn();
                break;
            }
            default:
                qWarning() << "Login failed!" << reply->error() << data;
                break;
        }
    });
}

void QBringApi::loadLists()
{
    QNetworkRequest request{QUrl( QString("%1/bringusers/%3/lists").arg(url, _uuid))};
    setHeaders(request);

    auto reply = _nm->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply] {
        reply->deleteLater();

        auto data = reply->readAll();

        switch (reply->error()) {
            case QNetworkReply::NoError: {
                qInfo() << "Successfully" << data.constData();
                auto json = QJsonDocument::fromJson(data).object();

                for(const auto& i : json["lists"].toArray()) {
                    auto list = i.toObject();
                    _lists[list["name"].toString()] = list["listUuid"].toString();
                }

                qInfo() << "Lists:" << _lists;
                emit listsLoaded();

                break;
            }
            default:
                qWarning() << "Failed!" << reply->error() << data;
                break;
        }
    });
}

const QMap<QString, QString> &QBringApi::lists() const
{
    return _lists;
}

void QBringApi::safeItem(const QString& listUuid, const QString &item, const QString &specification)
{
    QNetworkRequest request{QUrl(QString("%1/bringlists/%2").arg(url, listUuid))};
    setPutHeaders(request);

    QUrlQuery params;

    params.addQueryItem("purchase", item);
    params.addQueryItem("specification", specification);

    auto* reply = _nm->put(request, params.query(QUrl::FullyEncoded).toUtf8());

    connect(reply, &QNetworkReply::finished, this, [reply] {
        reply->deleteLater();
        auto data = reply->readAll();

        switch (reply->error()) {
            case QNetworkReply::NoError: {
                qInfo() << "Successfully" << data.constData();
                break;
            }
            default:
                qWarning() << "Failed!" << reply->error() << data;
                break;
        }
    });
}

void QBringApi::setHeaders(QNetworkRequest &request)
{
    request.setRawHeader("Authorization", QString("Bearer %1").arg(_accessToken).toUtf8());
    request.setRawHeader("X-BRING-API-KEY", "cof4Nc6D8saplXjE3h3HXqHH8m7VU2i1Gs0g85Sp");
    request.setRawHeader("X-BRING-CLIENT-SOURCE", "webApp");
    request.setRawHeader("X-BRING-API-KEY", "cof4Nc6D8saplXjE3h3HXqHH8m7VU2i1Gs0g85Sp");
    request.setRawHeader("X-BRING-CLIENT", "webApp");
    request.setRawHeader("X-BRING-COUNTRY", "DE");

    request.setRawHeader("X-BRING-USER-UUID", _uuid.toUtf8());
}

void QBringApi::setPutHeaders(QNetworkRequest &request)
{
    setHeaders(request);
    request.setRawHeader("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8");
}

void QBringApi::setPostHeaders(QNetworkRequest &request)
{
    setHeaders(request);
    request.setRawHeader("Content-Type", "application/json; charset=UTF-8");
}
