// MAINWINDOW.CPP

void MainWindow::createCommunicationClient(const QString &robot_ns, const QString &host, quint16 port)
{
    qDebug() << "Hacked";
    this->isConnected_ = true;

    // အရင် thread နဲ့ client ကို ရှင်းလင်းခြင်း
    if (communicationThread_ && communicationThread_->isRunning()) {
        // Thread ကို ဖြတ်ဖို့ စောင့်ဆိုင်းခြင်း
        communicationThread_->quit();
        communicationThread_->wait();
        
        // deleteLater ကို ခေါ်ရင် communication_ object ကို thread က delete လုပ်ပေးပါလိမ့်မယ်။
        // communication_->deleteLater(); // (သို့) QThread::finished မှာ ချိတ်ဆက်ထားရင် ပိုကောင်း)
    }
    delete communicationThread_;
    communicationThread_ = nullptr;
    communication_ = nullptr;
    
    // -------------------------------------------------------------
    // ⭐ 1. Thread အသစ် ဖန်တီးပါ။ (Main Thread တွင်)
    communicationThread_ = new QThread(this);

    // ⭐ 2. RosBridgeClient Worker Object ကို ဖန်တီးပါ။ (Main Thread တွင်)
    //    parent ကို 'nullptr' ပေးပါ၊ ဘာလို့လဲဆိုတော့ QObject::moveToThread() ကို သုံးချင်လို့ပါ။
    communication_ = new RosBridgeClient(robot_ns, host, port, nullptr); 

    // ⭐ 3. Worker Object ကို Thread အသစ်ဆီ ရွှေ့ပါ။
    communication_->moveToThread(communicationThread_);

    // ⭐ 4. Signals & Slots များ ချိတ်ဆက်ခြင်း
    
    // A. Worker Thread ကနေ Main Thread ကို Data ပို့ခြင်း (Thread-Safe)
    connect(communication_, &RosBridgeClient::receivedTopicMessage, 
            this, &MainWindow::onReceivedTopicMessage, Qt::QueuedConnection);

    // B. Main Thread ကနေ Worker Thread ကို Command ပို့ခြင်း
    //    (ဥပမာ: connection စတင်ရန်)
    //    Thread စတင်ပြီးမှသာ connectToServer() ကို ခေါ်သင့်ပါတယ်။
    connect(communicationThread_, &QThread::started, 
            communication_, &RosBridgeClient::connectToServer, Qt::QueuedConnection);
            
    // C. Resource ရှင်းလင်းခြင်း (Clean-up)
    // Thread ပြီးသွားရင် Worker object ကို အလိုအလျောက် ဖျက်ပေးဖို့
    connect(communicationThread_, &QThread::finished, 
            communication_, &QObject::deleteLater);
            
    // Worker object က disconnected signal ထုတ်လွှင့်ရင် Thread ကို ရပ်ဖို့
    connect(communication_, &RosBridgeClient::disconnected, 
            communicationThread_, &QThread::quit); 
            
    // ⭐ 5. Thread ကို စတင်ပါ။ (ဒီအချိန်မှသာ Client ရဲ့ Event Loop စပါလိမ့်မယ်)
    communicationThread_->start();
    // -------------------------------------------------------------
}

void MainWindow::~MainWindow()
{
    // Program ပိတ်ရင် Thread ကို စနစ်တကျ ရပ်ဖို့ သေချာပါစေ
    if (communicationThread_ && communicationThread_->isRunning()) {
        communicationThread_->quit();
        communicationThread_->wait(); 
    }
    delete ui;
}