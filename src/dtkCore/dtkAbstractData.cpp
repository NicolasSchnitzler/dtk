/* dtkAbstractData.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Nov  7 16:01:09 2008 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Apr  4 08:47:38 2012 (+0200)
 *           By: tkloczko
 *     Update #: 400
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataFactory.h>
#include <dtkCore/dtkAbstractDataReader.h>
#include <dtkCore/dtkAbstractDataWriter.h>
#include <dtkCore/dtkAbstractDataConverter.h>
#include <dtkCore/dtkAbstractDataSerializer.h>
#include <dtkCore/dtkAbstractDataDeserializer.h>

#include <dtkLog/dtkLog.h>

#include <QtGui>

class dtkAbstractDataPrivate
{
public:
    QMap<QString, bool> readers;
    QMap<QString, bool> writers;
    QMap<QString, bool> converters;
    QMap<QString, bool> serializers;
    QMap<QString, bool> deserializers;

    QString     path;
    QStringList paths;

    int numberOfChannels;

    QList<QImage> thumbnails;
};

dtkAbstractData::dtkAbstractData(dtkAbstractData *parent) : dtkAbstractObject(parent), d(new dtkAbstractDataPrivate)
{
    d->numberOfChannels = 0;
}

dtkAbstractData::dtkAbstractData(const dtkAbstractData& data) : dtkAbstractObject(), d(new dtkAbstractDataPrivate)
{
    this->setParent(data.parent());

    d->readers    = data.d->readers;
    d->writers    = data.d->writers;
    d->converters = data.d->converters;
    d->serializers = data.d->serializers;
    d->deserializers = data.d->deserializers;
}

dtkAbstractData::~dtkAbstractData(void)
{
    delete d;

    d = NULL;
}

void dtkAbstractData::addReader(const QString& reader)
{
    d->readers.insert(reader, false);
}

void dtkAbstractData::addWriter(const QString& writer)
{
    d->writers.insert(writer, false);
}

void dtkAbstractData::addConverter(const QString& converter)
{
    d->converters.insert(converter, false);
}

void dtkAbstractData::addSerializer(const QString& serializer)
{
    d->serializers.insert(serializer, false);
}

void dtkAbstractData::addDeserializer(const QString& deserializer)
{
    d->deserializers.insert(deserializer, false);
}

void dtkAbstractData::enableReader(const QString& reader)
{
    QMap<QString, bool>::iterator it(d->readers.find(reader));

    if (it != d->readers.end())
        it.value() = true;
    else
        dtkDebug() << this->identifier() << " has no such reader: " << reader;
}

void dtkAbstractData::disableReader(const QString& reader)
{
    QMap<QString, bool>::iterator it(d->readers.find(reader));

    if (it != d->readers.end())
        it.value() = false;
}

void dtkAbstractData::enableWriter(const QString& writer)
{
    QMap<QString, bool>::iterator it(d->writers.find(writer));

    if (it != d->writers.end())
        it.value() = true;
    else
        dtkDebug() << this->identifier() << " has no such writer: " << writer;
}

void dtkAbstractData::disableWriter(const QString& writer)
{
    QMap<QString, bool>::iterator it(d->writers.find(writer));

    if (it != d->writers.end())
        it.value() = false;
}

void dtkAbstractData::enableConverter(const QString& converter)
{
    QMap<QString, bool>::iterator it(d->converters.find(converter));

    if (it != d->converters.end())
        it.value() = true;
    else
        dtkDebug() << this->identifier() << " has no such converter: " << converter;
}

void dtkAbstractData::disableConverter(const QString& converter)
{
    QMap<QString, bool>::iterator it(d->converters.find(converter));

    if (it != d->converters.end())
        it.value() = false;
}

void dtkAbstractData::enableSerializer(const QString& serializer)
{
    QMap<QString, bool>::iterator it(d->serializers.find(serializer));

    if (it != d->serializers.end())
        it.value() = true;
    else
        dtkDebug() << this->identifier() << " has no such serializer: " << serializer;
}

void dtkAbstractData::disableSerializer(const QString& serializer)
{
    QMap<QString, bool>::iterator it(d->serializers.find(serializer));

    if (it != d->serializers.end())
        it.value() = false;
}

void dtkAbstractData::enableDeserializer(const QString& deserializer)
{
    QMap<QString, bool>::iterator it(d->deserializers.find(deserializer));

    if (it != d->deserializers.end())
        it.value() = true;
    else
        dtkDebug() << this->identifier() << " has no such deserializer: " << deserializer;
}

void dtkAbstractData::disableDeserializer(const QString& deserializer)
{
    QMap<QString, bool>::iterator it(d->deserializers.find(deserializer));

    if (it != d->deserializers.end())
        it.value() = false;
}

dtkAbstractDataReader *dtkAbstractData::reader(const QString& type)
{
    QMap<QString, bool>::const_iterator it(d->readers.find(type));

    if (it != d->readers.end() && (it.value() == true))
        return dtkAbstractDataFactory::instance()->reader(type);
    else 
        return NULL;
}

dtkAbstractDataWriter *dtkAbstractData::writer(const QString& type)
{
    QMap<QString, bool>::const_iterator it(d->writers.find(type));

    if (it != d->writers.end() && (it.value() == true))
        return dtkAbstractDataFactory::instance()->writer(type);
    else 
        return NULL;
}

dtkAbstractDataConverter *dtkAbstractData::converter(const QString& type)
{
    QMap<QString, bool>::const_iterator it(d->converters.find(type));

    if (it != d->converters.end() && (it.value() == true))
        return dtkAbstractDataFactory::instance()->converter(type);
    else 
        return NULL;
}

dtkAbstractDataSerializer *dtkAbstractData::serializer(const QString& type)
{
    QMap<QString, bool>::const_iterator it(d->serializers.find(type));

    if (it != d->serializers.end() && (it.value() == true))
        return dtkAbstractDataFactory::instance()->serializer(type);
    else
        return NULL;
}

dtkAbstractDataDeserializer *dtkAbstractData::deserializer(const QString& type)
{
    QMap<QString, bool>::const_iterator it(d->deserializers.find(type));

    if (it != d->deserializers.end() && (it.value() == true))
        return dtkAbstractDataFactory::instance()->deserializer(type);
    else
        return NULL;
}


int dtkAbstractData::numberOfChannels(void)
{
    return d->numberOfChannels;
}

void dtkAbstractData::setNumberOfChannels(int number)
{
    d->numberOfChannels = number;
}

void dtkAbstractData::update(void)
{
    DTK_DEFAULT_IMPLEMENTATION;
}

bool dtkAbstractData::read(const QString& file)
{
    bool read = false;

    dtkAbstractDataFactory *factoryInstance = dtkAbstractDataFactory::instance();

    for (QMap<QString, bool>::const_iterator it(d->readers.begin()); it != d->readers.end(); ++it) {

        if (it.value()) {
            QScopedPointer<dtkAbstractDataReader> reader( factoryInstance->reader(it.key()));

            if (!reader)
                continue;

            reader->setData(this);

            if (reader->read(file)) { 
                read = true;
                break;
            }
        }
    }
    
    if (read) {
        if (d->path.isEmpty())
            d->path = file;
        if (!d->paths.contains(file))
            d->paths << file;
    }

    return read;
}

bool dtkAbstractData::read(const QStringList& files)
{
    bool read = false;

    dtkAbstractDataFactory *factoryInstance = dtkAbstractDataFactory::instance();

    for (QMap<QString, bool>::const_iterator it(d->readers.begin()); it != d->readers.end(); ++it) {

        if (it.value()) {

            QScopedPointer<dtkAbstractDataReader> reader(factoryInstance->reader(it.key()));

            if (!reader) 
                continue;

            reader->setData(this);

            if (reader->read(files)) { 
                read = true;
                break;
            }
        }
    }

    if(read) {
        foreach(QString file, files) {
            if(!d->paths.contains(file))
                d->paths << file;
        }
    }

    return read;
}

bool dtkAbstractData::write(const QString& file)
{
    bool written = false;

    dtkAbstractDataFactory *factoryInstance = dtkAbstractDataFactory::instance();

    for (QMap<QString, bool>::const_iterator it(d->writers.begin()); it != d->writers.end(); ++it) {

        if (it.value()) {

            QScopedPointer<dtkAbstractDataWriter> writer(factoryInstance->writer(it.key()));

            if (!writer) 
                continue;

            writer->setData(this);

            if (writer->write(file)) { 
                written = true;
                break;
            }
        }
    }

    return written;
}

bool dtkAbstractData::write(const QStringList& files)
{
    bool written = false;

    dtkAbstractDataFactory *factoryInstance = dtkAbstractDataFactory::instance();

    for (QMap<QString, bool>::const_iterator it(d->writers.begin()); it != d->writers.end(); ++it) {

        if (it.value()) {

            QScopedPointer<dtkAbstractDataWriter> writer( factoryInstance->writer(it.key()));

            if (!writer)
                continue;

            writer->setData(this);

            if(writer->write(files)) { 
                written = true;
                break;
            }
        }
    }

    return written;
}


dtkAbstractData *dtkAbstractData::convert(const QString& toType)
{
    dtkAbstractData *conversion = NULL;

    for (QMap<QString, bool>::const_iterator it(d->converters.begin()); it!= d->converters.end() && !conversion ; ++it) {

        if (it.value()) {

            QScopedPointer<dtkAbstractDataConverter> converter( dtkAbstractDataFactory::instance()->converter(it.key()));

            if (converter && converter->canConvert(toType)) {
                converter->setData(this);
                conversion = converter->convert();

                if(conversion) {

                    foreach(QString metaDataKey, this->metaDataList())
                        conversion->addMetaData(metaDataKey, this->metaDataValues(metaDataKey));

                    foreach(QString propertyKey, this->propertyList())
                        conversion->addProperty(propertyKey, this->propertyValues(propertyKey));
                }
            }
        }
    }

    return conversion;
}

QByteArray *dtkAbstractData::serialize(void)
{
    QByteArray *array = NULL;

    for (QMap<QString, bool>::const_iterator it(d->serializers.begin()); it!= d->serializers.end() && array == NULL ; ++it) {

        if (it.value()) {

            QScopedPointer<dtkAbstractDataSerializer> serializer( dtkAbstractDataFactory::instance()->serializer(it.key()));

            if (serializer) {

                array = serializer->serialize(this);
                if (array != NULL) {
                    break;
                }
            }
        }
    }
    return array;
}

bool dtkAbstractData::deserialize(const QByteArray &array)
{

    bool deserialized = false;

    for (QMap<QString, bool>::const_iterator it(d->deserializers.begin()); it!= d->deserializers.end() && !deserialized ; ++it) {

        if (it.value()) {

            QScopedPointer<dtkAbstractDataDeserializer> deserializer( dtkAbstractDataFactory::instance()->deserializer(it.key()));

            if (deserializer) {
                deserializer->setData(this);

                deserialized = deserializer->deserialize(array);
                if(deserialized) {
                    break;
                }
            }
        }
    }
    return deserialized;
}

QString dtkAbstractData::path(void)
{
    return d->path;
}

QStringList dtkAbstractData::paths(void)
{
    return d->paths;
}

QImage& dtkAbstractData::thumbnail(void) const
{
    QImage *image = new QImage(128, 128, QImage::Format_RGB32);
    
    QPainter painter(image);
    painter.setRenderHints(QPainter::Antialiasing);
    painter.setPen(Qt::gray);
    painter.fillRect(image->rect(), Qt::black);
    
    d->thumbnails << (*image);

    return (*image);
}

QList<QImage>& dtkAbstractData::thumbnails(void) const
{
    return d->thumbnails;
}

void *dtkAbstractData::output(void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return NULL;
}

void *dtkAbstractData::output(int channel)
{
    DTK_DEFAULT_IMPLEMENTATION;
    DTK_UNUSED(channel);

    return NULL;
}

void *dtkAbstractData::data(void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return NULL;
}

void *dtkAbstractData::data(int channel)
{
    DTK_DEFAULT_IMPLEMENTATION;
    DTK_UNUSED(channel);

    return NULL;
}

double dtkAbstractData::parameter(int channel)
{
    DTK_DEFAULT_IMPLEMENTATION;
    DTK_UNUSED(channel);

    return -1;
}

void dtkAbstractData::setParameter(int parameter)
{
    DTK_DEFAULT_IMPLEMENTATION;
    DTK_UNUSED(parameter);
}

void dtkAbstractData::setParameter(int parameter, int channel)
{
    DTK_DEFAULT_IMPLEMENTATION;
    DTK_UNUSED(parameter);
    DTK_UNUSED(channel);
}

void dtkAbstractData::setParameter(float parameter)
{
    DTK_DEFAULT_IMPLEMENTATION;
    DTK_UNUSED(parameter);
}

void dtkAbstractData::setParameter(float parameter, int channel)
{
    DTK_DEFAULT_IMPLEMENTATION;
    DTK_UNUSED(parameter);
    DTK_UNUSED(channel);
}

void dtkAbstractData::setParameter(double parameter)
{
    DTK_DEFAULT_IMPLEMENTATION;
    DTK_UNUSED(parameter);
}

void dtkAbstractData::setParameter(double parameter, int channel)
{
    DTK_DEFAULT_IMPLEMENTATION;
    DTK_UNUSED(parameter);
    DTK_UNUSED(channel);
}

void dtkAbstractData::setParameter(const QString& parameter)
{
    DTK_DEFAULT_IMPLEMENTATION;
    DTK_UNUSED(parameter);
}

void dtkAbstractData::setParameter(const QString& parameter, int channel)
{
    DTK_DEFAULT_IMPLEMENTATION;
    DTK_UNUSED(parameter);
    DTK_UNUSED(channel);
}

void dtkAbstractData::setParameter(dtkAbstractData *parameter)
{
    DTK_DEFAULT_IMPLEMENTATION;
    DTK_UNUSED(parameter);
}

void dtkAbstractData::setParameter(dtkAbstractData *parameter, int channel)
{
    DTK_DEFAULT_IMPLEMENTATION;
    DTK_UNUSED(parameter);
    DTK_UNUSED(channel);
}

void dtkAbstractData::setData(void* data)
{
    DTK_DEFAULT_IMPLEMENTATION;
    DTK_UNUSED(data);
}

void dtkAbstractData::setData(void* data, int channel)
{
    DTK_DEFAULT_IMPLEMENTATION;
    DTK_UNUSED(data);
    DTK_UNUSED(channel);
}

bool dtkAbstractData::casts(const QString& type)
{
    DTK_DEFAULT_IMPLEMENTATION;
    DTK_UNUSED(type);

    return false;
}

dtkAbstractData::operator bool (void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return false;
}

dtkAbstractData::operator int (void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return 0;
}

dtkAbstractData::operator float (void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return 0.0;
}

dtkAbstractData::operator double (void)
{
    DTK_DEFAULT_IMPLEMENTATION;

    return 0.0;
}

QDebug operator<<(QDebug debug, const dtkAbstractData& data)
{
    debug.nospace() << data.identifier();

    return debug.space();
}

QDebug operator<<(QDebug debug, dtkAbstractData *data)
{
    debug.nospace() << data->identifier();

    return debug.space();
}
