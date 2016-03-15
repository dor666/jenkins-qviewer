#include <QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(5,4,0)
    #define Q_VIEWER_LOGGING_CATEGORY(name, string, msgType) \
    Q_LOGGING_CATEGORY((name), (string), (msgType))
#else
    #define Q_VIEWER_LOGGING_CATEGORY(name, string, msgType) \
    Q_LOGGING_CATEGORY((name), (string))
#endif

namespace jenkinsQViewer {

#if QT_VERSION >= QT_VERSION_CHECK(5,5,0)
    #define qVCInfo(category, ...) qCInfo((category), __VA_ARGS__)
#else
    #define qVCInfo(category, ...) qCDebug((category), __VA_ARGS__)
#endif

} // namespace jenkinsQViewer {
