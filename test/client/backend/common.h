#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

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
