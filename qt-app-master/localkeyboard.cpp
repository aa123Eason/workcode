#include "localkeyboard.h"
#include "ui_localkeyboard.h"

localKeyboard::localKeyboard(QWidget *widget,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::localKeyboard)
{
    ui->setupUi(this);
    curWidget = widget;
    init();
}

localKeyboard::~localKeyboard()
{
    delete ui;
}

void localKeyboard::init()
{
    keymap();
    // 获取屏幕宽度

    int screenWidth = QApplication::desktop()->width();

    // 获取屏幕高度

    int screenHeight = QApplication::desktop()->height();

    // window size
    int width = this->width();
    int height = this->height();

    // 设置窗口位置

    this->move(this->x() + (screenWidth - width) / 2,this->y()+(screenHeight - height) / 2);

    this->setWindowFlags(Qt::WindowStaysOnTopHint|Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowModality(Qt::WindowModal);
    ui->label->installEventFilter(this);

    connect(ui->close,&QPushButton::clicked,this,&localKeyboard::hide);
}

void localKeyboard::closeEvent(QCloseEvent *event)
{
    if(this->isVisible())
        this->hide();
}


void localKeyboard::keymap()
{
    //number
    keyMap["0"] = ui->key_0;
    keyMap["1"] = ui->key_1;
    keyMap["2"] = ui->key_2;
    keyMap["3"] = ui->key_3;
    keyMap["4"] = ui->key_4;
    keyMap["5"] = ui->key_5;
    keyMap["6"] = ui->key_6;
    keyMap["7"] = ui->key_7;
    keyMap["8"] = ui->key_8;
    keyMap["9"] = ui->key_9;

    numberBtns.push_back(keyMap["0"]);
    numberBtns.push_back(keyMap["1"]);
    numberBtns.push_back(keyMap["2"]);
    numberBtns.push_back(keyMap["3"]);
    numberBtns.push_back(keyMap["4"]);
    numberBtns.push_back(keyMap["5"]);
    numberBtns.push_back(keyMap["6"]);
    numberBtns.push_back(keyMap["7"]);
    numberBtns.push_back(keyMap["8"]);
    numberBtns.push_back(keyMap["9"]);

    //word
    keyMap["a"] = ui->a;
    keyMap["b"] = ui->b;
    keyMap["c"] = ui->c;
    keyMap["d"] = ui->d;
    keyMap["e"] = ui->e;
    keyMap["f"] = ui->f;
    keyMap["g"] = ui->g;
    keyMap["h"] = ui->h;
    keyMap["i"] = ui->i;
    keyMap["j"] = ui->j;
    keyMap["k"] = ui->k;
    keyMap["l"] = ui->l;
    keyMap["m"] = ui->m;

    keyMap["n"] = ui->n;
    keyMap["o"] = ui->o;
    keyMap["p"] = ui->p;
    keyMap["q"] = ui->q;
    keyMap["r"] = ui->r;
    keyMap["s"] = ui->s;
    keyMap["t"] = ui->t;
    keyMap["u"] = ui->u;
    keyMap["v"] = ui->v;
    keyMap["w"] = ui->w;
    keyMap["x"] = ui->x;
    keyMap["y"] = ui->y;
    keyMap["z"] = ui->z;

    letterBtns.push_back(keyMap["a"]);
    letterBtns.push_back(keyMap["b"]);
    letterBtns.push_back(keyMap["c"]);
    letterBtns.push_back(keyMap["d"]);
    letterBtns.push_back(keyMap["e"]);
    letterBtns.push_back(keyMap["f"]);
    letterBtns.push_back(keyMap["g"]);
    letterBtns.push_back(keyMap["h"]);
    letterBtns.push_back(keyMap["i"]);
    letterBtns.push_back(keyMap["j"]);
    letterBtns.push_back(keyMap["k"]);
    letterBtns.push_back(keyMap["l"]);
    letterBtns.push_back(keyMap["m"]);
    letterBtns.push_back(keyMap["n"]);
    letterBtns.push_back(keyMap["o"]);
    letterBtns.push_back(keyMap["p"]);
    letterBtns.push_back(keyMap["q"]);
    letterBtns.push_back(keyMap["r"]);
    letterBtns.push_back(keyMap["s"]);
    letterBtns.push_back(keyMap["t"]);
    letterBtns.push_back(keyMap["u"]);
    letterBtns.push_back(keyMap["v"]);
    letterBtns.push_back(keyMap["w"]);
    letterBtns.push_back(keyMap["x"]);
    letterBtns.push_back(keyMap["y"]);
    letterBtns.push_back(keyMap["z"]);

    //function key
    keyMap["Caps"] = ui->Caps;
    keyMap["BackSpace"] = ui->BackSpace;
    keyMap["sym"] = ui->sym;
    keyMap["Space"] = ui->Space;
    keyMap["lctrl"] = ui->lctrl;
    keyMap["lalt"] = ui->lalt;
    keyMap["rctrl"] = ui->rctrl;
    keyMap["ralt"] = ui->ralt;
    keyMap["enter"] = ui->enter;

    funcBtns.push_back(keyMap["Caps"]);
    funcBtns.push_back(keyMap["BackSpace"]);
    funcBtns.push_back(keyMap["sym"]);
    funcBtns.push_back(keyMap["Space"]);
    funcBtns.push_back(keyMap["lctrl"]);
    funcBtns.push_back(keyMap["lalt"]);
    funcBtns.push_back(keyMap["rctrl"]);
    funcBtns.push_back(keyMap["ralt"]);
    funcBtns.push_back(keyMap["enter"]);

    for(int i=0;i<keyMap.count();++i)
    {
        QString keyName = keyMap.keys()[i];
        QPushButton *btn = keyMap[keyName];
//        btn->installEventFilter(this);
        btn->setAutoRepeat(true);
        btn->setAutoRepeatDelay(500);
    }

    for(int i=0;i<numberBtns.count();++i)
    {
        connect(numberBtns[i],&QPushButton::clicked,this,&localKeyboard::slotKeyNumberButtonClicked);
    }

    for(int i=0;i<letterBtns.count();++i)
    {
        connect(letterBtns[i],&QPushButton::clicked,this,&localKeyboard::slotKeyLetterButtonClicked);
    }

    for(int i=0;i<funcBtns.count();++i)
    {
        connect(funcBtns[i],&QPushButton::clicked,this,&localKeyboard::slotKeyButtonClicked);
    }

    sysKeyMap["0"]=Qt::Key_0;
    sysKeyMap["1"]=Qt::Key_1;
    sysKeyMap["2"]=Qt::Key_2;
    sysKeyMap["3"]=Qt::Key_3;
    sysKeyMap["4"]=Qt::Key_4;
    sysKeyMap["5"]=Qt::Key_5;
    sysKeyMap["6"]=Qt::Key_6;
    sysKeyMap["7"]=Qt::Key_7;
    sysKeyMap["8"]=Qt::Key_8;
    sysKeyMap["9"]=Qt::Key_9;

    sysKeyMap["a"]=Qt::Key_A;
    sysKeyMap["b"]=Qt::Key_B;
    sysKeyMap["c"]=Qt::Key_C;
    sysKeyMap["d"]=Qt::Key_D;
    sysKeyMap["e"]=Qt::Key_E;
    sysKeyMap["f"]=Qt::Key_F;
    sysKeyMap["g"]=Qt::Key_G;
    sysKeyMap["h"]=Qt::Key_H;
    sysKeyMap["i"]=Qt::Key_I;
    sysKeyMap["j"]=Qt::Key_J;
    sysKeyMap["k"]=Qt::Key_K;
    sysKeyMap["l"]=Qt::Key_L;
    sysKeyMap["m"]=Qt::Key_M;
    sysKeyMap["n"]=Qt::Key_N;
    sysKeyMap["o"]=Qt::Key_O;
    sysKeyMap["p"]=Qt::Key_P;
    sysKeyMap["q"]=Qt::Key_Q;
    sysKeyMap["r"]=Qt::Key_R;
    sysKeyMap["s"]=Qt::Key_S;
    sysKeyMap["t"]=Qt::Key_T;
    sysKeyMap["u"]=Qt::Key_U;
    sysKeyMap["v"]=Qt::Key_V;
    sysKeyMap["w"]=Qt::Key_W;
    sysKeyMap["x"]=Qt::Key_X;
    sysKeyMap["y"]=Qt::Key_Y;
    sysKeyMap["z"]=Qt::Key_Z;


    sysKeyMap.insert("~", Qt::Key_AsciiTilde);
    sysKeyMap.insert("`", Qt::Key_nobreakspace);
    sysKeyMap.insert("-", Qt::Key_Minus);
    sysKeyMap.insert("_", Qt::Key_Underscore);
    sysKeyMap.insert("+", Qt::Key_Plus);
    sysKeyMap.insert("=", Qt::Key_Equal);
    sysKeyMap.insert(",", Qt::Key_Comma);
    sysKeyMap.insert(".", Qt::Key_Period);
    sysKeyMap.insert("/", Qt::Key_Slash);
    sysKeyMap.insert("<", Qt::Key_Less);
    sysKeyMap.insert(">", Qt::Key_Greater);
    sysKeyMap.insert("?", Qt::Key_Question);
    sysKeyMap.insert("[", Qt::Key_BracketLeft);
    sysKeyMap.insert("]", Qt::Key_BracketRight);
    sysKeyMap.insert("{", Qt::Key_BraceLeft);
    sysKeyMap.insert("}", Qt::Key_BraceRight);
    sysKeyMap.insert("|", Qt::Key_Bar);
    sysKeyMap.insert("\\", Qt::Key_Backslash);
    sysKeyMap.insert(":", Qt::Key_Colon);
    sysKeyMap.insert(";", Qt::Key_Semicolon);
    sysKeyMap.insert("\"", Qt::Key_QuoteLeft);
    sysKeyMap.insert("'", Qt::Key_Apostrophe);


}

bool localKeyboard::eventFilter(QObject *o,QEvent *ev)
{
    if(o==ui->label)
    {
        switch (ev->type()) //这里更具事件的类型来判断，
        {
        case QEvent::MouseButtonPress:
        {
            QMouseEvent* e=static_cast<QMouseEvent*>(ev);//因为后续要访问鼠标的按键类型和位置，而父类QEvent是没有这些成员的，所以要强转
            if(e->button()==Qt::LeftButton)
            {
                pos=e->pos();//记录此时的鼠标左键点击位置
            }
        }
            break;
        case QEvent::MouseMove:
        {
            QMouseEvent* e=static_cast<QMouseEvent*>(ev);//因为后续要访问鼠标的按键类型和位置，而父类QEvent是没有这些成员的，所以要强转
            if(e->buttons()==Qt::LeftButton)
            {
                int x,y;
                x=e->pos().x()-pos.x();
                y=e->pos().y()-pos.y();
                this->move(this->x()+x,this->y()+y);
            }
        }
            break;
        default:
            break;


        }
    }
        return QWidget::eventFilter(o,ev);
}

void localKeyboard::slotKeyButtonClicked()
{
    if(!curWidget)return;
    QPushButton* pbtn = (QPushButton*)sender();
    qDebug()<<__LINE__<<pbtn->text()<<endl;
    QString objectName = pbtn->objectName();
    if (pbtn->text().contains("BackSpace")) {
        QKeyEvent keyPress(QEvent::KeyPress, Qt::Key_Backspace, Qt::NoModifier);
        QKeyEvent keyRelease(QEvent::KeyRelease, Qt::Key_Backspace, Qt::NoModifier);
        QApplication::sendEvent(curWidget->focusWidget(), &keyPress);
        QApplication::sendEvent(curWidget->focusWidget(), &keyRelease);
    }
    else if (pbtn->text().contains("Caps")) {
        if (pbtn->isChecked()) {
            for (auto pbtnKey : letterBtns) {

                if(pbtnKey->whatsThis().split("|").count()>1)
                    pbtnKey->setText(pbtnKey->whatsThis().split("|")[0].toUpper());
            }
        }
        else {
            for (auto pbtnKey : letterBtns) {
                if(pbtnKey->whatsThis().split("|").count()>1)
                    pbtnKey->setText(pbtnKey->whatsThis().split("|")[0].toLower());
            }
        }
    }
    else if(pbtn == ui->Space) {
        QKeyEvent keyPress(QEvent::KeyPress, Qt::Key_Space, Qt::NoModifier, " ");
        QKeyEvent keyRelease(QEvent::KeyRelease, Qt::Key_Space, Qt::NoModifier, " ");
        QApplication::sendEvent(curWidget->focusWidget(), &keyPress);
        QApplication::sendEvent(curWidget->focusWidget(), &keyRelease);
    }
    else if (pbtn->text().contains("Enter")) {
        QKeyEvent keyPress(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
        QKeyEvent keyRelease(QEvent::KeyRelease, Qt::Key_Enter, Qt::NoModifier);
        QApplication::sendEvent(curWidget->focusWidget(), &keyPress);
        QApplication::sendEvent(curWidget->focusWidget(), &keyRelease);
    }
    else if (pbtn->text().contains("Ctrl")) {
        QKeyEvent keyPress(QEvent::KeyPress, Qt::Key_Control, Qt::NoModifier);
        QKeyEvent keyRelease(QEvent::KeyRelease, Qt::Key_Control, Qt::NoModifier);
        QApplication::sendEvent(curWidget->focusWidget(), &keyPress);
        QApplication::sendEvent(curWidget->focusWidget(), &keyRelease);
    }
    else if (pbtn->text().contains("Alt")) {
        QKeyEvent keyPress(QEvent::KeyPress, Qt::Key_Alt, Qt::NoModifier);
        QKeyEvent keyRelease(QEvent::KeyRelease, Qt::Key_Alt, Qt::NoModifier);
        QApplication::sendEvent(curWidget->focusWidget(), &keyPress);
        QApplication::sendEvent(curWidget->focusWidget(), &keyRelease);
    }
    else if(pbtn->text().contains("符"))
    {
        if (pbtn->isChecked()) {
            qDebug()<<__LINE__<<"开启符号键转换"<<endl;
            for (auto pbtnKey : letterBtns) {

                if(pbtnKey->whatsThis().split("|").count()>1)
                    pbtnKey->setText(pbtnKey->whatsThis().split("|")[1]);
            }

            for (auto pbtnKey : numberBtns) {

                if(pbtnKey->whatsThis().split("|").count()>1)
                    pbtnKey->setText(pbtnKey->whatsThis().split("|")[1]);
            }
        }
        else {
            qDebug()<<__LINE__<<"关闭符号键转换"<<endl;

            for (auto pbtnKey : letterBtns) {
                if(pbtnKey->whatsThis().split("|").count()>1)
                    pbtnKey->setText(pbtnKey->whatsThis().split("|")[0].toLower());
            }

            for (auto pbtnKey : numberBtns) {
                if(pbtnKey->whatsThis().split("|").count()>1)
                    pbtnKey->setText(pbtnKey->whatsThis().split("|")[0]);
            }
        }
    }

//    取消组合键按下
//    if (!pbtn->text().contains("Ctrl") && !pbtn->text().contains("Alt") ) {
//        if (ui->Caps->isChecked()) {
//            ui->Caps->setChecked(false);
//            for (auto pbtnKey : letterBtns) {
//                pbtnKey->setText(pbtnKey->text().toLower());
//            }
//        }

//        if (ui->sym->isChecked()) {
//            ui->sym->setChecked(false);
//            for (auto pbtnKey : letterBtns) {
//                if(pbtnKey->whatsThis().split("|").count()>1)
//                    pbtnKey->setText(pbtnKey->whatsThis().split("|")[0].toLower());
//            }

//            for (auto pbtnKey : numberBtns) {
//                if(pbtnKey->whatsThis().split("|").count()>1)
//                    pbtnKey->setText(pbtnKey->whatsThis().split("|")[0]);
//            }
//        }


//        if (ui->lctrl->isChecked())
//            ui->lctrl->setChecked(false);
//        if (ui->rctrl->isChecked())
//            ui->rctrl->setChecked(false);
//        if (ui->lalt->isChecked())
//            ui->lalt->setChecked(false);
//        if (ui->ralt->isChecked())
//            ui->ralt->setChecked(false);
//    }



}

void localKeyboard::slotKeyLetterButtonClicked()
{
    if(!curWidget)return;
    QPushButton* pbtn = (QPushButton*)sender();
    qDebug()<<__LINE__<<pbtn->text()<<endl;
    if (pbtn->text() >= 'a' && pbtn->text() <= 'z') {
        QKeyEvent keyPress(QEvent::KeyPress, int(pbtn->text().at(0).toLatin1()) - 32, Qt::NoModifier, pbtn->text());
        QKeyEvent keyRelease(QEvent::KeyRelease, int(pbtn->text().at(0).toLatin1()) - 32, Qt::NoModifier, pbtn->text());
        QApplication::sendEvent(curWidget->focusWidget(), &keyPress);
        QApplication::sendEvent(curWidget->focusWidget(), &keyRelease);
    }
    else if (pbtn->text() >= 'A' && pbtn->text() <= 'Z') {
        QKeyEvent keyPress(QEvent::KeyPress, int(pbtn->text().at(0).toLatin1()), Qt::NoModifier, pbtn->text());
        QKeyEvent keyRelease(QEvent::KeyRelease, int(pbtn->text().at(0).toLatin1()), Qt::NoModifier, pbtn->text());
        QApplication::sendEvent(curWidget->focusWidget(), &keyPress);
        QApplication::sendEvent(curWidget->focusWidget(), &keyRelease);
    }
    //取消组合键按下
//    if (!pbtn->text().contains("Ctrl") && !pbtn->text().contains("Alt") ) {
//        if (ui->Caps->isChecked()) {
//            ui->Caps->setChecked(false);
//            for (auto pbtnKey : letterBtns) {
//                pbtnKey->setText(pbtnKey->text().toLower());
//            }
//        }

//        if (ui->sym->isChecked()) {
//            ui->sym->setChecked(false);
//            for (auto pbtnKey : letterBtns) {
//                if(pbtnKey->whatsThis().split("|").count()>1)
//                    pbtnKey->setText(pbtnKey->whatsThis().split("|")[0].toLower());
//            }

//            for (auto pbtnKey : numberBtns) {
//                if(pbtnKey->whatsThis().split("|").count()>1)
//                    pbtnKey->setText(pbtnKey->whatsThis().split("|")[0]);
//            }
//        }


//        if (ui->lctrl->isChecked())
//            ui->lctrl->setChecked(false);
//        if (ui->rctrl->isChecked())
//            ui->rctrl->setChecked(false);
//        if (ui->lalt->isChecked())
//            ui->lalt->setChecked(false);
//        if (ui->ralt->isChecked())
//            ui->ralt->setChecked(false);
//    }

}

void localKeyboard::slotKeyNumberButtonClicked()
{
    if(!curWidget)return;
    QPushButton* pbtn = (QPushButton*)sender();
    qDebug()<<__LINE__<<pbtn->text()<<endl;
    QKeyEvent keyPress(QEvent::KeyPress, pbtn->text().toInt() + 48, Qt::NoModifier, pbtn->text());
    QKeyEvent keyRelease(QEvent::KeyRelease, pbtn->text().toInt() + 48, Qt::NoModifier, pbtn->text());
    QApplication::sendEvent(curWidget->focusWidget(), &keyPress);
    QApplication::sendEvent(curWidget->focusWidget(), &keyRelease);
    //取消组合键按下
//    if (!pbtn->text().contains("Ctrl") && !pbtn->text().contains("Alt") ) {
//        if (ui->Caps->isChecked()) {
//            ui->Caps->setChecked(false);
//            for (auto pbtnKey : letterBtns) {
//                pbtnKey->setText(pbtnKey->text().toLower());
//            }
//        }

//        if (ui->sym->isChecked()) {
//            ui->sym->setChecked(false);
//            for (auto pbtnKey : letterBtns) {
//                if(pbtnKey->whatsThis().split("|").count()>1)
//                    pbtnKey->setText(pbtnKey->whatsThis().split("|")[0].toLower());
//            }

//            for (auto pbtnKey : numberBtns) {
//                if(pbtnKey->whatsThis().split("|").count()>1)
//                    pbtnKey->setText(pbtnKey->whatsThis().split("|")[0]);
//            }
//        }


//        if (ui->lctrl->isChecked())
//            ui->lctrl->setChecked(false);
//        if (ui->rctrl->isChecked())
//            ui->rctrl->setChecked(false);
//        if (ui->lalt->isChecked())
//            ui->lalt->setChecked(false);
//        if (ui->ralt->isChecked())
//            ui->ralt->setChecked(false);
//    }

}
