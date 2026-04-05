class MenuLayer : cocos2d::CCLayer, FLAlertLayerProtocol, GooglePlayDelegate {
    // static MenuLayer* create() = ios 0x2655a4;
    // virtual ~MenuLayer();

    static MenuLayer* get() = inline;
    static cocos2d::CCScene* scene(bool isVideoOptionsOpen) = win 0x333900, imac 0x38ffc0, m1 0x3164d8, ios 0x2663a4;

    virtual bool init() = win 0x333a90, imac 0x390180, m1 0x316688, ios 0x266500;
    virtual void keyBackClicked() = win 0x335f80, imac 0x3920a0, m1 0x318504, ios 0x26800c;
    virtual void keyDown(cocos2d::enumKeyCodes key, double timestamp) = win 0x336360, imac 0x392370, m1 0x318780, ios 0x268294;
    virtual void googlePlaySignedIn() = win 0x335980, imac 0x391e10, m1 0x3182b8, ios 0x267e70;
    virtual void FLAlert_Clicked(FLAlertLayer* layer, bool btn2) = win 0x336150, imac 0x3920d0, m1 0x318510, ios 0x2680cc;

    void endGame() = win 0x3363d0, imac 0x392340, m1 0x318774, ios 0x268278;
    void firstNetworkTest() = win 0x3352f0, imac 0x391bf0, m1 0x3180e0, ios 0x267d54;
    void onAchievements(cocos2d::CCObject* sender) = win 0x335820, imac 0x391650, m1 0x317afc, ios 0x267870;
    void onCreator(cocos2d::CCObject* sender) = win 0x335d30, imac 0x3915d0, m1 0x317a88, ios 0x267820;
    void onDaily(cocos2d::CCObject* sender) = win 0x3350a0, imac 0x391b90, m1 0x31807c, ios 0x267cf0;
    void onDiscord(cocos2d::CCObject* sender) = win 0x335720, imac 0x391880, m1 0x317d6c, ios 0x267abc;
    void onEveryplay(cocos2d::CCObject* sender) = win inline, imac 0x391f20, m1 0x3183b0, ios inline;
    void onFacebook(cocos2d::CCObject* sender) = win 0x3356a0, imac 0x391800, m1 0x317cdc, ios 0x267a2c;
    void onFreeLevels(cocos2d::CCObject* sender) = imac 0x391ff0, m1 0x318458;
    void onFullVersion(cocos2d::CCObject* sender) = imac 0x392010, m1 0x318478;
    void onGameCenter(cocos2d::CCObject* sender) = win inline, imac 0x391c80, m1 0x318150, ios 0x267d68;
    void onGarage(cocos2d::CCObject* sender) = win 0x335dc0, imac 0x391550, m1 0x317a14, ios 0x2677d0;
    void onGooglePlayGames(cocos2d::CCObject* sender) = win inline, imac 0x391de0, m1 0x318290, ios inline;
    void onMoreGames(cocos2d::CCObject* sender) = win 0x335740, imac 0x3918a0, m1 0x317d90, ios 0x267ae0;
    void onMyProfile(cocos2d::CCObject* sender) = win 0x3355e0, imac 0x3919f0, m1 0x317eec, ios 0x267b64;
    void onNewgrounds(cocos2d::CCObject* sender) = win 0x335e50, imac 0x391710, m1 0x317be0, ios 0x267954;
    void onOptions(cocos2d::CCObject* sender) = win 0x335a70, imac 0x391690, m1 0x317b48, ios 0x2678bc;
    void onOptionsInstant() = win 0x335a80, imac 0x390120, m1 0x316620, ios 0x2664f8;
    void onPlay(cocos2d::CCObject* sender) = win 0x3359d0, imac 0x3914c0, m1 0x31799c, ios 0x26777c;
    void onQuit(cocos2d::CCObject* sender) = win 0x335f90, imac 0x391920, m1 0x317e14, ios 0x268010;
    void onRobTop(cocos2d::CCObject* sender) = win 0x335680, imac 0x3917e0, m1 0x317cb8, ios 0x267a08;
    void onStats(cocos2d::CCObject* sender) = win 0x335c10, imac 0x3916d0, m1 0x317b94, ios 0x267908;
    void onTrailer(cocos2d::CCObject* sender) = win inline, imac 0x392060, m1 0x3184c4, ios inline;
    void onTwitch(cocos2d::CCObject* sender) = win 0x335700, imac 0x391860, m1 0x317d48, ios 0x267a98;
    void onTwitter(cocos2d::CCObject* sender) = win 0x3356c0, imac 0x391820, m1 0x317d00, ios 0x267a50;
    void onYouTube(cocos2d::CCObject* sender) = win 0x3356e0, imac 0x391840, m1 0x317d24, ios 0x267a74;
    void openOptions(bool videoOptions) = win 0x335a90, imac 0x391f30, m1 0x3183b4, ios 0x267f68;
    void showGCQuestion() = win inline, imac 0x391cd0, m1 0x318198, ios 0x267db0;
    void showMeltdownPromo() = win inline, imac 0x391c10, m1 0x3180f4, ios inline;
    void showTOS() = win 0x3354c0, imac 0x391bb0, m1 0x318098, ios 0x267d0c;
    void syncPlatformAchievements(float dt) = win inline, imac 0x391ee0, m1 0x318384, ios 0x267f3c;
    void tryShowAd(float dt) = win inline, imac 0x391c20, m1 0x3180f8, ios inline;
    void updateUserProfileButton() = win 0x3351f0, imac 0x391a50, m1 0x317f4c, ios 0x267bc4;
    void videoOptionsClosed() = win inline, imac 0x391fd0, m1 0x31844c, ios 0x268000;
    void videoOptionsOpened() = win inline, imac 0x391fb0, m1 0x31843c, ios 0x267ff0;
    void willClose() = win 0x3360b0, imac 0x38fce0, m1 0x3162dc, ios 0x26632c;

    bool m_showingTOS;
    cocos2d::CCSprite* m_gpSprite;
    cocos2d::CCSprite* m_viewProfileSprite;
    cocos2d::CCLabelBMFont* m_profileLabel;
    CCMenuItemSpriteExtra* m_profileButton;
    void* m_unknown;
    MenuGameLayer* m_menuGameLayer;
}

class LoadingLayer : cocos2d::CCLayer {
    LoadingLayer() = inline;
    ~LoadingLayer() = win inline, imac 0x3a3690, m1 0x328258, ios 0x1d6e70;

    static LoadingLayer* create(bool refresh) = win inline, imac 0x3a25b0, m1 0x3271fc, ios 0x1d5f40;
    static cocos2d::CCScene* scene(bool refresh) = win 0x32e3b0, imac 0x3a24b0, m1 0x327124, ios 0x1d5efc;

    const char* getLoadingString() = win 0x32f7f0, imac 0x3a2e00, m1 0x327a3c, ios 0x1d66c8;
    bool init(bool refresh) = win 0x32e4c0, imac 0x3a2690, m1 0x3272bc, ios 0x1d5ff4;
    void loadAssets() = win 0x32ed60, imac 0x3a2ec0, m1 0x327b18, ios 0x1d67a4;
    void loadingFinished() = win inline, imac 0x3a3660, m1 0x328224, ios 0x1d6e3c;
    void updateProgress(int progress) = win 0x32ece0, imac 0x3a2e60, m1 0x327aac, ios 0x1d6738;

    bool m_unknown;
    bool m_unknown2;
    int m_loadStep;
    cocos2d::CCLabelBMFont* m_caption;
    TextArea* m_textArea;
    cocos2d::CCSprite* m_sliderBar;
    float m_sliderGrooveXPos;
    float m_sliderGrooveHeight;
    bool m_fromRefresh;
}

class CreatorLayer : cocos2d::CCLayer, cocos2d::CCSceneTransitionDelegate, DialogDelegate {
    // virtual ~CreatorLayer();
    CreatorLayer() = inline;

    static CreatorLayer* create() = win inline, imac 0x297520, m1 0x2376d8, ios 0xb4fc8;
    static cocos2d::CCScene* scene() = win 0x9bfd0, imac 0x297400, m1 0x2375f4, ios 0xb4f70;

    virtual bool init() = win 0x9c140, imac 0x297600, m1 0x237794, ios 0xb5078;
    virtual void keyBackClicked() = win 0x9f9c0, imac 0x2999c0, m1 0x2399b8, ios 0xb6c08;
    virtual void sceneWillResume() = win 0x9f860, imac 0x299940, m1 0x239948, ios 0xb6bc8;
    virtual void dialogClosed(DialogLayer* layer) = win 0x9f740, imac 0x299840, m1 0x239844, ios 0xb6b38;

    bool canPlayOnlineLevels() = win inline, imac 0x2997d0, m1 0x2397f0, ios inline;
    void checkQuestsStatus() = win 0x9f620, imac 0x298e50, m1 0x238eb0, ios 0xb6418;
    void onAdventureMap(cocos2d::CCObject* sender) = win 0x9e250, imac 0x2988a0, m1 0x238904, ios 0xb60a8;
    void onBack(cocos2d::CCObject* sender) = win 0x9f800, imac 0x299770, m1 0x239790, ios 0xb6ad8;
    void onChallenge(cocos2d::CCObject* sender) = win 0x9e420, imac 0x298910, m1 0x238998, ios 0xb6120;
    void onDailyLevel(cocos2d::CCObject* sender) = win 0x9e3c0, imac 0x298940, m1 0x2389d0, ios 0xb6158;
    void onEventLevel(cocos2d::CCObject* sender) = win 0x9e400, imac 0x298980, m1 0x238a10, ios 0xb6198;
    void onFameLevels(cocos2d::CCObject* sender) = win inline, imac 0x2997e0, m1 0x2397f8, ios inline;
    void onFeaturedLevels(cocos2d::CCObject* sender) = win 0x9d910, imac 0x2989f0, m1 0x238a78, ios 0xb6200;
    void onGauntlets(cocos2d::CCObject* sender) = win 0x9dc00, imac 0x2989a0, m1 0x238a30, ios 0xb61b8;
    void onLeaderboards(cocos2d::CCObject* sender) = win 0x9d720, imac 0x2984e0, m1 0x2384fc, ios 0xb5d38;
    void onMapPacks(cocos2d::CCObject* sender) = win 0x9db40, imac 0x298ad0, m1 0x238b40, ios 0xb62c8;
    void onMultiplayer(cocos2d::CCObject* sender) = win 0x9dc80, imac 0x298530, m1 0x238544, ios 0xb5d80;
    void onMyLevels(cocos2d::CCObject* sender) = win 0x9d310, imac 0x298b70, m1 0x238bc8, ios 0xb6350;
    void onOnlineLevels(cocos2d::CCObject* sender) = win 0x9dac0, imac 0x298b30, m1 0x238b8c, ios 0xb6314;
    void onOnlyFullVersion(cocos2d::CCObject* sender) = imac 0x298c50, m1 0x238c90;
    void onPaths(cocos2d::CCObject* sender) = win 0x9d9d0, imac 0x298a50, m1 0x238ac4, ios 0xb624c;
    void onSavedLevels(cocos2d::CCObject* sender) = win 0x9d520, imac 0x298410, m1 0x23843c, ios 0xb5c78;
    void onSecretVault(cocos2d::CCObject* sender) = win 0x9e590, imac 0x298f50, m1 0x238fb0, ios 0xb64f4;
    void onTopLists(cocos2d::CCObject* sender) = win 0x9d9f0, imac 0x298a70, m1 0x238ae0, ios 0xb6268;
    void onTreasureRoom(cocos2d::CCObject* sender) = win 0x9eda0, imac 0x299170, m1 0x2391fc, ios 0xb66c4;
    void onWeeklyLevel(cocos2d::CCObject* sender) = win 0x9e3e0, imac 0x298960, m1 0x2389f0, ios 0xb6178;

    cocos2d::CCSprite* m_secretDoorSprite;
    cocos2d::CCSprite* m_questsSprite;
    int m_vaultDialogIndex;
    int m_versusDialogIndex;
}