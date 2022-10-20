//
// Created by Vitalii Arkusha on 18.10.2022.
//

#ifndef PS_MANAGEMENT_COMMUNICATIONMETRICS_H
#define PS_MANAGEMENT_COMMUNICATIONMETRICS_H

#include <QtGlobal>
#include <QQueue>
#include <QDebug>

struct CommunicationMetrics {
    int errorCount = 0;
    int droppedCount = 0;
    int responseTimeoutCount = 0;

    void setQueueSize(int size) {
        mQueueSizes.enqueue(size);
        if (mQueueSizes.count() > 10) {
            mQueueSizes.dequeue();
        }
    }

    int queueSize() const {
        double ratio = 1.0;
        foreach(int size, mQueueSizes) {
            ratio = ratio > 1e5 ? 1e5 : (ratio * size);
        }
        ratio = ratio / mQueueSizes.count();
        return qMin(int(ratio), 101);
    }

private:
    QQueue<int> mQueueSizes;
};


#endif //PS_MANAGEMENT_COMMUNICATIONMETRICS_H
