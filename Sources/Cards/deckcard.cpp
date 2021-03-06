#include "deckcard.h"
#include "../utility.h"
#include "../themehandler.h"
#include <QtWidgets>


bool DeckCard::drawClassColor = false;
bool DeckCard::drawSpellWeaponColor = false;
int DeckCard::cardHeight = 35;


DeckCard::DeckCard(QString code, bool outsider)
{
    setCode(code);
    listItem = nullptr;
    total = remaining = 1;
    topManaLimit = bottomManaLimit = false;
    special = false;
    createdByCode = "";
    id = 0;
    this->outsider = outsider;
}


DeckCard::~DeckCard()
{

}


bool DeckCard::isOutsider()
{
    return this->outsider;
}


void DeckCard::setManaLimit(bool top)
{
    if(top)
    {
        if(!topManaLimit)
        {
            topManaLimit = true;
            draw();
        }
    }
    else
    {
        if(!bottomManaLimit)
        {
            bottomManaLimit = true;
            draw();
        }
    }
}


void DeckCard::resetManaLimits()
{
    bool redraw = topManaLimit || bottomManaLimit;
    topManaLimit = bottomManaLimit = false;
    if(redraw)  draw();
}


void DeckCard::setCode(QString code)
{
    this->code = code;

    if(!code.isEmpty())
    {
        cost = Utility::getCardAttribute(code, "cost").toInt();
        type = Utility::getTypeFromCode(code);
        name = Utility::getCardAttribute(code, "name").toString();
        rarity = Utility::getRarityFromCode(code);
        cardClass = Utility::getClassFromCode(code);
        cardRace = Utility::getRaceFromCode(code);
    }
    else
    {
        cost = -1;
        type = INVALID_TYPE;
        name = "unknown";
        rarity = INVALID_RARITY;
        cardClass = INVALID_CLASS;
        cardRace = INVALID_RACE;
    }
}


//Reusamos cost/type/name/rarity para createdByCode
void DeckCard::setCreatedByCode(QString code)
{
    if(!this->code.isEmpty()) return;

    this->createdByCode = code;

    if(!createdByCode.isEmpty())
    {
        cost = Utility::getCardAttribute(code, "cost").toInt();
        type = Utility::getTypeFromCode(code);
        name = Utility::getCardAttribute(code, "name").toString();
        rarity = Utility::getRarityFromCode(code);
        cardClass = Utility::getClassFromCode(code);
        cardRace = Utility::getRaceFromCode(code);
    }
    else
    {
        cost = -1;
        type = INVALID_TYPE;
        name = "unknown";
        rarity = INVALID_RARITY;
        cardClass = INVALID_CLASS;
        cardRace = INVALID_RACE;
    }
}


QString DeckCard::getCreatedByCode()
{
    return this->createdByCode;
}


void DeckCard::draw()
{
    QPixmap canvas;

    if(!this->createdByCode.isEmpty() && this->code.isEmpty())
    {
        canvas = drawCustomCard(this->createdByCode, "BY:");
    }
    else
    {
        if(remaining > 0)   canvas = draw(remaining, false, BLACK);
        else                canvas = draw(total, false, BLACK);
    }


    if(remaining == 0)      this->listItem->setIcon(QIcon(QIcon(canvas).pixmap(
                                                              canvas.size(), QIcon::Disabled, QIcon::On)));
    else                    this->listItem->setIcon(QIcon(canvas));
}


QPixmap DeckCard::draw(int total, bool drawRarity, QColor nameColor, bool resize, QString manaText)
{
    QFont font(ThemeHandler::cardsFont());
    font.setBold(true);
    font.setKerning(true);
#ifdef Q_OS_WIN
            font.setLetterSpacing(QFont::AbsoluteSpacing, -2);
#else
            font.setLetterSpacing(QFont::AbsoluteSpacing, -1);
#endif

    QPixmap canvas(CARD_SIZE);
    canvas.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&canvas);
        //Antialiasing
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.setRenderHint(QPainter::TextAntialiasing);

        //Borders behind
        if(ThemeHandler::manaLimitBehind())
        {
            QPixmap pixmap(ThemeHandler::manaLimitFile());
            int pixmapHMid = pixmap.height()/2;
            int pixmapW = pixmap.width();
            if(topManaLimit)        painter.drawPixmap(0, 0, pixmap, 0, pixmapHMid, pixmapW, pixmapHMid);
            if(bottomManaLimit)     painter.drawPixmap(0, 35-pixmapHMid, pixmap, 0, 0, pixmapW, pixmapHMid);
        }

        //Card
        QRectF target;
        QRectF source;
        if(name == "unknown")                   source = QRectF(0,0,100,25);
        else if(type==MINION)                   source = QRectF(46,72,100,25);
        else                                    source = QRectF(46,98,100,25);
        if(total == 1 && rarity != LEGENDARY)   target = QRectF(113,6,100,25);
        else                                    target = QRectF(100,6,100,25);

        if(name == "unknown")   painter.drawPixmap(target, QPixmap(ThemeHandler::unknownFile()), source);
        else    painter.drawPixmap(target, QPixmap(Utility::hscardsPath() + "/" + code + ".png"), source);

        //Background and #cards
        painter.setPen(QPen(BLACK));

        if(nameColor!=BLACK)                            painter.setBrush(nameColor);
        else if(drawRarity)                             painter.setBrush(getRarityColor());
        else if(outsider)                               painter.setBrush(VIOLET);
        else if(drawSpellWeaponColor && type==SPELL)    painter.setBrush(YELLOW);
        else if(drawSpellWeaponColor && type==WEAPON)   painter.setBrush(ORANGE);
        else                                            painter.setBrush(WHITE);


        int maxNameLong;
        if(total == 1 && rarity != LEGENDARY)
        {
            maxNameLong = 174;
            painter.drawPixmap(0,0,QPixmap(drawClassColor?ThemeHandler::bgCard1File(cardClass):ThemeHandler::bgCard1File()));
        }
        else
        {
            maxNameLong = 155;
            painter.drawPixmap(0,0,QPixmap(drawClassColor?ThemeHandler::bgCard2File(cardClass):ThemeHandler::bgCard2File()));

            if(total > 1)
            {
                font.setPixelSize(22);//16pt
                Utility::drawShadowText(painter, font, QString::number(total), 202, 19, true);
            }
            else
            {
                painter.drawPixmap(195, 8, QPixmap(ThemeHandler::starFile()));
            }
        }


        //Name and mana
        if(name == "unknown")
        {
            font.setPixelSize(14);//10pt
            painter.setPen(QPen(BLACK));
            painter.setBrush(QColor(ThemeHandler::themeColor1()));
            Utility::drawShadowText(painter, font, "Unknown", 34, 20, false);
        }
        else
        {
            //Name
            int fontSize = 15;
            font.setPixelSize(fontSize);//11pt

            QFontMetrics fm(font);
            int textWide = fm.width(name);
            while(textWide>maxNameLong)
            {
                fontSize--;
                font.setPixelSize(fontSize);//<11pt
                fm = QFontMetrics(font);
                textWide = fm.width(name);
            }

            Utility::drawShadowText(painter, font, name, 34, 20, false);

            //Mana cost
            if(manaText.isEmpty())
            {
                int manaSize = cost>9?26:static_cast<int>(18+1.5*cost);
                font.setPixelSize(manaSize);//20pt | 14 + cost
                Utility::drawShadowText(painter, font, QString::number(cost), 13, 20, true);
            }
            //Custom mana number
            else
            {
                painter.setBrush(WHITE);
                font.setPixelSize(16);
                Utility::drawShadowText(painter, font, manaText, 13, 20, true);
            }

            //Borders front
            if(!ThemeHandler::manaLimitBehind())
            {
                QPixmap pixmap(ThemeHandler::manaLimitFile());
                int pixmapHMid = pixmap.height()/2;
                int pixmapW = pixmap.width();
                if(topManaLimit)        painter.drawPixmap(0, 0, pixmap, 0, pixmapHMid, pixmapW, pixmapHMid);
                if(bottomManaLimit)     painter.drawPixmap(0, 35-pixmapHMid, pixmap, 0, 0, pixmapW, pixmapHMid);
            }
        }
    painter.end();

    //Adapt to size
    if(resize)  return resizeCardHeight(canvas);
    else        return canvas;
}


QPixmap DeckCard::drawCustomCard(QString customCode, QString customText)
{
    QFont font(ThemeHandler::cardsFont());
    font.setBold(true);
    font.setKerning(true);
#ifdef Q_OS_WIN
        font.setLetterSpacing(QFont::AbsoluteSpacing, -2);
#else
        font.setLetterSpacing(QFont::AbsoluteSpacing, -1);
#endif

    QPixmap canvas(CARD_SIZE);
    canvas.fill(Qt::transparent);
    QPainter painter;
    painter.begin(&canvas);
        //Antialiasing
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.setRenderHint(QPainter::TextAntialiasing);

        //Borders behind
        if(ThemeHandler::manaLimitBehind())
        {
            QPixmap pixmap(ThemeHandler::manaLimitFile());
            int pixmapHMid = pixmap.height()/2;
            int pixmapW = pixmap.width();
            if(topManaLimit)        painter.drawPixmap(0, 0, pixmap, 0, pixmapHMid, pixmapW, pixmapHMid);
            if(bottomManaLimit)     painter.drawPixmap(0, 35-pixmapHMid, pixmap, 0, 0, pixmapW, pixmapHMid);
        }

        //Card
        QRectF target;
        QRectF source;

        QFileInfo cardFI(Utility::hscardsPath() + "/" + customCode + ".png");
        if(cardFI.exists())
        {
            if(type==MINION)        source = QRectF(46,72,100,25);
            else                    source = QRectF(46,98,100,25);
            if(total == 1)          target = QRectF(113,6,100,25);
            else                    target = QRectF(100,6,100,25);
            painter.drawPixmap(target, QPixmap(Utility::hscardsPath() + "/" + customCode + ".png"), source);
        }
        else
        {
            source = QRectF(63,18,100,25);
            target = QRectF(113,6,100,25);
            painter.drawPixmap(target, QPixmap(ThemeHandler::unknownFile()), source);
        }

        //Background
        if(total == 1)
        {
            painter.drawPixmap(0,0,QPixmap(ThemeHandler::handCardBYFile()));
        }
        else
        {
            painter.drawPixmap(0,0,QPixmap(ThemeHandler::handCardBYFile2()));
        }

        //BY
        int fontSize = 15;
        font.setPixelSize(fontSize);//11pt
        QFontMetrics fm(font);
        int customTextWide = fm.width(customText);
        painter.setBrush(BLACK);
        painter.setPen(QPen(WHITE));
        Utility::drawShadowText(painter, font, customText, 10, 20, false);

        //Name
        int nameWide = fm.width(name);
        int maxNameLong = 194 - customTextWide + (total==1?0:-19);
        while(nameWide>maxNameLong)
        {
            fontSize--;
            font.setPixelSize(fontSize);//<11pt
            fm = QFontMetrics(font);
            nameWide = fm.width(name);
        }

        painter.setPen(QPen(BLACK));

        if(outsider)                                    painter.setBrush(VIOLET);
        else if(drawSpellWeaponColor && type==SPELL)    painter.setBrush(YELLOW);
        else if(drawSpellWeaponColor && type==WEAPON)   painter.setBrush(ORANGE);
        else                                            painter.setBrush(WHITE);

        Utility::drawShadowText(painter, font, name, 14 + customTextWide, 20, false);

        //#cards
        if(total > 1)
        {
            font.setPixelSize(22);//16pt
            Utility::drawShadowText(painter, font, QString::number(total), 202, 19, true);
        }

        //Borders front
        if(!ThemeHandler::manaLimitBehind())
        {
            QPixmap pixmap(ThemeHandler::manaLimitFile());
            int pixmapHMid = pixmap.height()/2;
            int pixmapW = pixmap.width();
            if(topManaLimit)        painter.drawPixmap(0, 0, pixmap, 0, pixmapHMid, pixmapW, pixmapHMid);
            if(bottomManaLimit)     painter.drawPixmap(0, 35-pixmapHMid, pixmap, 0, 0, pixmapW, pixmapHMid);
        }
    painter.end();

    return resizeCardHeight(canvas);
}


QPixmap DeckCard::resizeCardHeight(QPixmap &canvas)
{
    if(cardHeight==35)  return canvas;

    if(cardHeight<25)
    {
        canvas = canvas.copy(0,0+6,218,35-10);
        return canvas.scaled(QSize(218,cardHeight), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    }
    else if(cardHeight>35)
    {
        return canvas.scaled(QSize(218,cardHeight), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);
    }
    else    //25<=cardHeight<35
    {
        int reducePx = 35 - cardHeight;//1-10
        int topPx = reducePx/2+1;
        return canvas.copy(0,0+topPx,218,35-reducePx);
    }
}


QColor DeckCard::getRarityColor()
{
    if(rarity == COMMON)            return WHITE;
    else if(rarity == RARE)         return BLUE;
    else if(rarity == EPIC)         return VIOLET;
    else if(rarity == LEGENDARY)    return ORANGE;
    else                            return BLACK;
}


QString DeckCard::getCode()
{
    return code;
}


CardType DeckCard::getType()
{
    return type;
}


QString DeckCard::getName()
{
    return name;
}


int DeckCard::getCost()
{
    return cost;
}

CardRarity DeckCard::getRarity()
{
    return rarity;
}


CardClass DeckCard::getCardClass()
{
    return cardClass;
}


CardRace DeckCard::getRace()
{
    return cardRace;
}


void DeckCard::setDrawClassColor(bool value)
{
    DeckCard::drawClassColor = value;
}


void DeckCard::setDrawSpellWeaponColor(bool value)
{
    DeckCard::drawSpellWeaponColor = value;
}


void DeckCard::setCardHeight(int value)
{
    DeckCard::cardHeight = value;
}


int DeckCard::getCardHeight()
{
    return DeckCard::cardHeight;
}


int DeckCard::getCardWidth()
{
    return (cardHeight<=35)?218:static_cast<int>(cardHeight/35.0*218);
}
