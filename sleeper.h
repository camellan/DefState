#ifndef SLEEPER_H
#define SLEEPER_H
class Sleeper: public QThread
{
    public:
        static void msleep(int ms)
        {
            QThread::msleep(ms);
        }
};
#endif // SLEEPER_H
