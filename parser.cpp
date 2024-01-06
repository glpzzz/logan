#include "parser.h"

Parser::Parser()
{

}

Parser::Parser(const QString &newName, const QString &newPattern, const QStringList &newHeaderLabels)
{
    this->setName(newName);
    this->setPattern(newPattern);
    this->setHeaderLabels(newHeaderLabels);
}

QString Parser::getName() const
{
    return name;
}

void Parser::setName(const QString &newName)
{
    name = newName;
}

QString Parser::getPattern() const
{
    return pattern;
}

void Parser::setPattern(const QString &newPattern)
{
    pattern = newPattern;
}

QStringList Parser::getHeaderLabels() const
{
    return headerLabels;
}

void Parser::setHeaderLabels(const QStringList &newHeaderLabels)
{
    headerLabels = newHeaderLabels;
}
