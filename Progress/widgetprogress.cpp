#include "widgetprogress.h"

#include <QPainter>
#include <QRect>
#include <QFontMetrics>
#include <QFont>
#include <QtMath>

// test
#include <QDebug>
WidgetProgress::WidgetProgress(QWidget *parent)
    : QWidget{parent}
{
    init();
}

void WidgetProgress::paintEvent(QPaintEvent *event)
{
    if (mProgressType == P_Cicle)
    {
        drawCicle();
    }
    else
    {
        drawLiner();
    }
    QWidget::paintEvent(event);
}

void WidgetProgress::init()
{
    QFont font("Microsoft YaHei", 10);
    QFontMetrics metrics(font);
    QRect fontRect = metrics.boundingRect("测试文本");

    mProgressHeight = fontRect.height() * 0.48;

    // 根据文字大小确定环形进度条宽度
    mCicleWidth = fontRect.height() * 0.42;

    setVisible(true);
}

void WidgetProgress::drawLiner()
{
    QPainter *painter = new QPainter;
    painter->begin(this);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(mBackgroundColor));

    int width = this->width();
    int height = this->height();

    QFont font("Microsoft YaHei", 10);
    QFontMetrics metrics(font);
    QRect fontRect = metrics.boundingRect("100%");

    int labelWidth = mShowLabel ? fontRect.width() : 0;

    // 绘制背景
    painter->drawRoundedRect(QRect(0, (height - mProgressHeight) / 2.0, width - labelWidth, mProgressHeight), mProgressHeight / 2.0, mProgressHeight / 2.0);

    // 根据当前值绘制实际进度
    painter->setBrush(QBrush(mColor));
    double rate = mValue * 1.00 / (mMaxValue - mMinValue) / 100.00;
    painter->drawRoundedRect(QRect(0, (height - mProgressHeight) / 2.0, width * rate - labelWidth, mProgressHeight), mProgressHeight / 2.0, mProgressHeight / 2.0);

    // 绘制进度文本
    if (mShowLabel)
    {
        painter->setPen(QPen(QColor(mLabelColor), 2, Qt::SolidLine));
        painter->setBrush(QBrush(QColor(mLabelColor)));
        painter->drawText(QRect(width - labelWidth, 0 , labelWidth, height), Qt::AlignCenter, QString("%1%").arg(QString::number(rate * 100, 'f', 2)));
    }

    painter->end();

    delete painter;
}

void WidgetProgress::drawCicle()
{
    QPainter *painter = new QPainter;
    painter->begin(this);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(mBackgroundColor, mCicleWidth, Qt::SolidLine, Qt::FlatCap, Qt::RoundJoin));
    painter->setBrush(QBrush(mBackgroundColor));

    int width = this->width();
    int height = this->height();

    int cicleWidth = qMin(width, height);
    int cicleHeight = qMin(width, height);

    // 绘制背景
    int startAngle = 90 * 16;
    int spanAngle = 360 * 16;
    painter->drawArc(QRect(mCicleWidth + (width - cicleWidth) / 2.0, mCicleWidth + (height - cicleHeight) / 2.0, cicleWidth - mCicleWidth * 2, cicleHeight - mCicleWidth * 2), startAngle, spanAngle);

    // 根据当前值绘制实际进度
    painter->setPen(QPen(mColor, mCicleWidth, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    double rate = mValue * 1.00 / (mMaxValue - mMinValue) / 100.00;
    spanAngle = spanAngle * rate * -1;
    painter->drawArc(QRect(mCicleWidth + (width - cicleWidth) / 2.0, mCicleWidth + (height - cicleHeight) / 2.0, cicleWidth - mCicleWidth * 2, cicleHeight - mCicleWidth * 2), startAngle, spanAngle);
    // 绘制进度文本
    QFont font("Microsoft YaHei", 12);
    QFontMetrics metrics(font);
    QRect fontRect = metrics.boundingRect("100%");

    // 太小不利于文本显示
    if (mShowLabel && (fontRect.width() * 2.0 < cicleWidth))
    {
        painter->setFont(font);
        painter->setPen(QPen(QColor(mLabelColor), 3, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        painter->setBrush(QBrush(QColor(mLabelColor)));
        painter->drawText(QRect(0, 0 , width, height), Qt::AlignCenter, QString("%1%").arg(QString::number(rate * 100, 'f', 2)));
    }

    painter->end();

    delete painter;
}

int WidgetProgress::getCicleWidth() const
{
    return mCicleWidth;
}

void WidgetProgress::setCicleWidth(int newCicleWidth)
{
    mCicleWidth = newCicleWidth;
}

int WidgetProgress::getProgressType() const
{
    return mProgressType;
}

void WidgetProgress::setProgressType(int newProgressType)
{
    mProgressType = newProgressType;
}

const QColor &WidgetProgress::getLabelColor() const
{
    return mLabelColor;
}

void WidgetProgress::setLabelColor(const QColor &newLabelColor)
{
    mLabelColor = newLabelColor;
}

bool WidgetProgress::getShowLabel() const
{
    return mShowLabel;
}

void WidgetProgress::setShowLabel(bool newShowLable)
{
    mShowLabel = newShowLable;
}

const QColor &WidgetProgress::getColor() const
{
    return mColor;
}

void WidgetProgress::setColor(const QColor &newColor)
{
    mColor = newColor;
}

float WidgetProgress::getValue() const
{
    return mValue / 100.00;
}

void WidgetProgress::setValue(float newValue)
{
    if (newValue > mMaxValue)
    {
        mValue = mMaxValue * 100.00;
    }
    else
    {
        mValue = newValue * 100.00;
    }

    update();
}
