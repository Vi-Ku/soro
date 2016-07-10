#ifndef SORO_LOGGER_H
#define SORO_LOGGER_H

#include <QtCore>

#include "soro_global.h"

/* These macros allow more consise output to the root logger
 */
#define LOG_D(Tag, Msg) Logger::rootLogger()->d(Tag, Msg)
#define LOG_I(Tag, Msg) Logger::rootLogger()->i(Tag, Msg)
#define LOG_W(Tag, Msg) Logger::rootLogger()->w(Tag, Msg)
#define LOG_E(Tag, Msg) Logger::rootLogger()->e(Tag, Msg)

namespace Soro {

/* A very simple logging implementation. Provides functionality to output
 * log data to a file as well signaling when messages are published.
 *
 * This class is reentrant, because QTextStream is reentrant and mutexes are expensive.
 * Create separate logger instances for separate threads.
 */
class Logger: public QObject {
    Q_OBJECT
public:
    enum Level {
        LogLevelDisabled = 0,
        LogLevelError,
        LogLevelWarning,
        LogLevelInformation,
        LogLevelDebug
    };

private:
    static const QString _levelFormatters[];
    static const QString _levelFormattersHTML[];
    QFile* _file = NULL;
    QTextStream* _fileStream = NULL;
    void publish(Level level, QString tag, QString message);

    Level _maxFileLevel = LogLevelDebug;
    Level _maxQtLogLevel = LogLevelDisabled;

    // These format the log messages to the desiered text appearance
    QStringList _textFormat;
    QStringList _qtLoggerFormat;

    // Singleton root logger
    static Logger *_root;

public:
    Logger(QObject *parent = NULL);
    ~Logger();

    /* Set a file to direct log output to
     */
    bool setLogfile(QString fileName);

    /* Closes any existing log file and
     * stops output to it.
     */
    void closeLogfile();

    /* Gets a pointer to the global root logger instance.
     *
     * This class can still be instantiated and used
     * separately elsewhere.
     */
    static inline Logger* rootLogger() {
        return _root;
    }

    inline void d(QString tag, QString message) {
        publish(LogLevelDebug, tag, message);
    }

    inline void i(QString tag, QString message) {
        publish(LogLevelInformation, tag, message);
    }

    inline void w(QString tag, QString message) {
        publish(LogLevelWarning, tag, message);
    }

    inline void e(QString tag, QString message) {
        publish(LogLevelError, tag, message);
    }

    /* Specify the maximum level that will be written to an
     * output file.
     */
    void setMaxFileLevel(Logger::Level maxLevel);

    /* Specify the maximum level that will be forwarded to the
     * Qt logging system.
     */
    void setMaxQtLoggerLevel(Logger::Level maxLevel);

    /* Specify the formatting to be applied to log messages published to the
     * output file. The list should have 4 elements, with the first corresponding
     * to the highest log level (error) and the lowest corresponding to the lowest
     * (debug).
     *
     * Each item in the list should have 3 Qt-style placeholders (%1, %2, %3)
     * which correspond to, in order:
     * 1) The timestamp
     * 2) The tag
     * 3) The message
     */
    void setOutputFileTextFormat(const QStringList& format);

};

}

#endif // SORO_LOGGER_H
