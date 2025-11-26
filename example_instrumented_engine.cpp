/*
 * Ví dụ minh họa phân biệt 2 đối tượng this trong connect
 * 
 * File này giải thích rõ ràng về InstrumentedEngine và cách connect hoạt động
 */

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlError>
#include <QDebug>
#include <QObject>

// ============================================
// INSTRUMENTED ENGINE - Ví dụ thực tế
// ============================================

class InstrumentedEngine : public QQmlApplicationEngine {
    Q_OBJECT
    
public:
    explicit InstrumentedEngine(QObject *parent = nullptr)
        : QQmlApplicationEngine(parent) {
        
        qDebug() << "=== InstrumentedEngine Constructor ===";
        qDebug() << "this pointer:" << this;
        qDebug() << "this as QQmlApplicationEngine*:" << static_cast<QQmlApplicationEngine*>(this);
        qDebug() << "this as InstrumentedEngine*:" << static_cast<InstrumentedEngine*>(this);
        qDebug() << "Are they the same? YES - same memory address";
        qDebug() << "";
        
        // ============================================
        // PHÂN TÍCH CONNECT:
        // ============================================
        qDebug() << "=== Setting up connection ===";
        qDebug() << "connect(this, &QQmlApplicationEngine::warnings, this, &InstrumentedEngine::onWarnings)";
        qDebug() << "";
        qDebug() << "this (đầu tiên - SENDER):";
        qDebug() << "  - Address:" << this;
        qDebug() << "  - Type: InstrumentedEngine* (nhưng cũng là QQmlApplicationEngine*)";
        qDebug() << "  - Vai trò: Object phát signal 'warnings'";
        qDebug() << "  - Signal: QQmlApplicationEngine::warnings";
        qDebug() << "";
        qDebug() << "this (thứ hai - RECEIVER):";
        qDebug() << "  - Address:" << this;
        qDebug() << "  - Type: InstrumentedEngine*";
        qDebug() << "  - Vai trò: Object nhận signal và gọi slot";
        qDebug() << "  - Slot: InstrumentedEngine::onWarnings";
        qDebug() << "";
        qDebug() << "KẾT LUẬN: Cùng một object, nhưng vai trò khác nhau!";
        qDebug() << "";
        
        // Connect signal từ base class với slot trong derived class
        connect(this, &QQmlApplicationEngine::warnings, 
                this, &InstrumentedEngine::onWarnings);
        
        qDebug() << "Connection established successfully!";
        qDebug() << "";
    }
    
    // Method để test - load QML file có warnings
    void loadWithWarnings(const QUrl &url) {
        qDebug() << "=== Loading QML file (may generate warnings) ===";
        load(url);
    }
    
private slots:
    void onWarnings(const QList<QQmlError> &warnings) {
        qDebug() << "";
        qDebug() << "========================================";
        qDebug() << "=== SLOT CALLED: onWarnings ===";
        qDebug() << "========================================";
        qDebug() << "this pointer in slot:" << this;
        qDebug() << "Received" << warnings.size() << "warning(s)";
        qDebug() << "";
        
        for (int i = 0; i < warnings.size(); ++i) {
            const QQmlError &error = warnings[i];
            qDebug() << "Warning" << (i + 1) << ":";
            qDebug() << "  URL:" << error.url();
            qDebug() << "  Line:" << error.line();
            qDebug() << "  Column:" << error.column();
            qDebug() << "  Message:" << error.description();
            qDebug() << "";
        }
        
        qDebug() << "NOTE: This slot was called because:";
        qDebug() << "  1. QQmlApplicationEngine emitted 'warnings' signal";
        qDebug() << "  2. Connection forwarded it to this slot";
        qDebug() << "  3. Even though sender and receiver are the same object,";
        qDebug() << "     they have different roles in the connection";
        qDebug() << "========================================";
        qDebug() << "";
    }
};

// ============================================
// VÍ DỤ SỬ DỤNG
// ============================================

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    qDebug() << "========================================";
    qDebug() << "DEMO: Phân Biệt 2 Đối Tượng this";
    qDebug() << "========================================";
    qDebug() << "";
    
    // Tạo engine
    InstrumentedEngine engine;
    
    qDebug() << "";
    qDebug() << "=== Memory Layout ===";
    qDebug() << "InstrumentedEngine object contains:";
    qDebug() << "  [QQmlApplicationEngine part]";
    qDebug() << "    - Has signal: warnings";
    qDebug() << "    - this (sender) points here";
    qDebug() << "  [InstrumentedEngine part]";
    qDebug() << "    - Has slot: onWarnings";
    qDebug() << "    - this (receiver) points here";
    qDebug() << "";
    qDebug() << "Both 'this' pointers point to the SAME memory address!";
    qDebug() << "";
    
    // Load QML file (có thể tạo warnings)
    // Uncomment để test:
    // engine.loadWithWarnings(QUrl("qrc:/main.qml"));
    
    qDebug() << "=== Explanation ===";
    qDebug() << "";
    qDebug() << "Khi QQmlApplicationEngine phát signal 'warnings':";
    qDebug() << "  1. Signal được emit từ base class part";
    qDebug() << "  2. Qt's signal-slot system tìm connections";
    qDebug() << "  3. Tìm thấy connection đến slot onWarnings";
    qDebug() << "  4. Gọi slot onWarnings trong derived class part";
    qDebug() << "  5. Cả sender và receiver đều là cùng một object";
    qDebug() << "";
    qDebug() << "Đây là pattern 'self-connection' hoặc 'signal interception'";
    qDebug() << "";
    
    return app.exec();
}

// ============================================
// SO SÁNH VỚI CÁC CÁCH VIẾT KHÁC
// ============================================

/*
 * CÁCH 1: Viết rõ ràng hơn với biến cục bộ
 */
void example1() {
    class MyEngine : public QQmlApplicationEngine {
        Q_OBJECT
    public:
        MyEngine() {
            // Đặt tên rõ ràng
            QQmlApplicationEngine *sender = this;
            InstrumentedEngine *receiver = this;
            
            connect(sender, &QQmlApplicationEngine::warnings,
                    receiver, &InstrumentedEngine::onWarnings);
        }
    };
}

/*
 * CÁCH 2: Sử dụng lambda
 */
void example2() {
    class MyEngine : public QQmlApplicationEngine {
        Q_OBJECT
    public:
        MyEngine() {
            // Lambda capture this - rõ ràng this là receiver
            connect(this, &QQmlApplicationEngine::warnings,
                    [this](const QList<QQmlError> &warnings) {
                        this->onWarnings(warnings);
                    });
        }
    private:
        void onWarnings(const QList<QQmlError> &warnings) {}
    };
}

/*
 * CÁCH 3: Connect với đối tượng khác
 */
void example3() {
    class WarningHandler : public QObject {
        Q_OBJECT
    public slots:
        void handleWarnings(const QList<QQmlError> &warnings) {}
    };
    
    class MyEngine : public QQmlApplicationEngine {
        Q_OBJECT
    public:
        MyEngine() {
            WarningHandler *handler = new WarningHandler(this);
            
            // this là sender, handler là receiver (KHÁC NHAU)
            connect(this, &QQmlApplicationEngine::warnings,
                    handler, &WarningHandler::handleWarnings);
        }
    };
}

#include "example_instrumented_engine.moc"
