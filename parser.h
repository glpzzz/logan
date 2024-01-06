#ifndef PARSER_H
#define PARSER_H

#include <QString>
#include <QStringList>


class Parser
{
public:
    Parser();
    Parser(const QString &newName, const QString &newPattern, const QStringList &newHeaderLabels);

    QString getPattern() const;
    void setPattern(const QString &newPattern);

    QStringList getHeaderLabels() const;
    void setHeaderLabels(const QStringList &newHeaderLabels);

    QString getName() const;
    void setName(const QString &newName);

private:
    QString name;
    QString pattern;
    QStringList headerLabels;

};

#endif // PARSER_H
