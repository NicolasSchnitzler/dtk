/* dtkJson.h ---
 *
 * Author: Nicolas Niclausse
 * Copyright (C) 2011 - Nicolas Niclausse, Inria.
 * Created: ven. août  5 10:01:48 2011 (+0200)
 * Version: $Id$
 * Last-Updated: ven. août  5 14:14:58 2011 (+0200)
 *           By: Nicolas Niclausse
 *     Update #: 26
 */

/* Commentary:
 * See credits at EOF.
 */

/* Change log:
 *
 */

#ifndef DTK_JSON_H
#define DTK_JSON_H

#include "dtkJsonExport.h"

#include <QVariant>
#include <QString>


class dtkJsonPrivate;

class DTKJSON_EXPORT dtkJson
{
    friend class dtkJsonPrivate;

public:

    enum JsonToken
    {
        JsonTokenNone = 0,
        JsonTokenCurlyOpen = 1,
        JsonTokenCurlyClose = 2,
        JsonTokenSquaredOpen = 3,
        JsonTokenSquaredClose = 4,
        JsonTokenColon = 5,
        JsonTokenComma = 6,
        JsonTokenString = 7,
        JsonTokenNumber = 8,
        JsonTokenTrue = 9,
        JsonTokenFalse = 10,
        JsonTokenNull = 11
    };


private:
    dtkJsonPrivate *p;

//private:
/*     dtkJson(const dtkJson& that); */

    static  QString sanitizeString(QString str);
    static  QByteArray join(const QList<QByteArray> &list, const QByteArray &sep);


public:

    /**
     * Parse a JSON string
     *
     * \param json The JSON data
     */
    static QVariant parse(const QString &json);

    /**
     * Parse a JSON string
     *
     * \param json The JSON data
     * \param success The success of the parsing
     */
    static QVariant parse(const QString &json, bool &success);

    /**
     * This method generates a textual JSON representation
     *
     * \param data The JSON data generated by the parser.
     * \param success The success of the serialization
     */
    static QByteArray serialize(const QVariant &data);

    /**
     * This method generates a textual JSON representation
     *
     * \param data The JSON data generated by the parser.
     * \param success The success of the serialization
     *
     * \return QByteArray Textual JSON representation
     */
    static QByteArray serialize(const QVariant &data, bool &success);

private:
    /**
     * Parses a value starting from index
     *
     * \param json The JSON data
     * \param index The start index
     * \param success The success of the parse process
     *
     * \return QVariant The parsed value
     */
    static QVariant parseValue(const QString &json, int &index, bool &success);

    /**
     * Parses an object starting from index
     *
     * \param json The JSON data
     * \param index The start index
     * \param success The success of the object parse
     *
     * \return QVariant The parsed object map
     */
    static QVariant parseObject(const QString &json, int &index, bool &success);

    /**
     * Parses an array starting from index
     *
     * \param json The JSON data
     * \param index The starting index
     * \param success The success of the array parse
     *
     * \return QVariant The parsed variant array
     */
    static QVariant parseArray(const QString &json, int &index, bool &success);

    /**
     * Parses a string starting from index
     *
     * \param json The JSON data
     * \param index The starting index
     * \param success The success of the string parse
     *
     * \return QVariant The parsed string
     */
    static QVariant parseString(const QString &json, int &index, bool &success);

    /**
     * Parses a number starting from index
     *
     * \param json The JSON data
     * \param index The starting index
     *
     * \return QVariant The parsed number
     */
    static QVariant parseNumber(const QString &json, int &index);

    /**
     * Get the last index of a number starting from index
     *
     * \param json The JSON data
     * \param index The starting index
     *
     * \return The last index of the number
     */
    static int lastIndexOfNumber(const QString &json, int index);

    /**
     * Skip unwanted whitespace symbols starting from index
     *
     * \param json The JSON data
     * \param index The start index
     */
    static void eatWhitespace(const QString &json, int &index);

    /**
     * Check what token lies ahead
     *
     * \param json The JSON data
     * \param index The starting index
     *
     * \return int The upcoming token
     */
    static int lookAhead(const QString &json, int index);

    /**
     * Get the next JSON token
     *
     * \param json The JSON data
     * \param index The starting index
     *
     * \return int The next JSON token
     */
    static int nextToken(const QString &json, int &index);
};

#endif
// /////////////////////////////////////////////////////////////////
// Credits
// /////////////////////////////////////////////////////////////////

/*

Originally written by Eeli Reilin <eeli@emicode.fi>, PUBLIC DOMAIN.

*/
