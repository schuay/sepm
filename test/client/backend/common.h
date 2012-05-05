#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

#define TEMP_SESSION_USER "TEMP_SESSION_USER@selinux.inso.tuwien.ac.at"

/**
 *  * Checks every 50 ms if spy has received a result, and returns if it did.
 *   * Otherwise, it aborts after 10 seconds.
 *    */
static void waitForResult(const QSignalSpy &spy, int count = 1)
{
    for (int i = 0; i < 200; i++) {
        QTest::qWait(50);
        if (spy.count() >= count)
            break;
    }
}
