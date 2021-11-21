#pragma once
#include "custom-types/shared/macros.hpp"
#include "UnityEngine/MonoBehaviour.hpp"
#include "questui/shared/CustomTypes/Components/List/CustomListTableData.hpp"
#include "HMUI/CurvedTextMeshPro.hpp"
#include "HMUI/ImageView.hpp"
#include "HMUI/ModalView.hpp"

#include "GlobalNamespace/CustomPreviewBeatmapLevel.hpp"
#include "System/Threading/Tasks/Task_1.hpp"
#include "UnityEngine/Sprite.hpp"


DECLARE_CLASS_CODEGEN(MultiQuestensions::UI, DownloadedSongsGSM, UnityEngine::MonoBehaviour,
    DECLARE_INSTANCE_FIELD(HMUI::ModalView*, modal);
    DECLARE_INSTANCE_FIELD(QuestUI::CustomListTableData*, list);
    DECLARE_INSTANCE_FIELD(GlobalNamespace::CustomPreviewBeatmapLevel*, lastDownloaded);

    DECLARE_INSTANCE_METHOD(void, DidActivate, bool firstActivation);
    DECLARE_INSTANCE_METHOD(void, OnEnable);
    DECLARE_INSTANCE_METHOD(void, Refresh);
    DECLARE_INSTANCE_METHOD(void, CreateCell, System::Threading::Tasks::Task_1<UnityEngine::Sprite*>* coverTask, GlobalNamespace::CustomPreviewBeatmapLevel* level);
    DECLARE_INSTANCE_METHOD(void, Delete);

    static DownloadedSongsGSM* instance;

    int selectedIdx;
    bool needSongRefresh;
    public:
    static DownloadedSongsGSM* get_Instance() { return instance; }

    static std::vector<std::string> mapQueue;

public:
    void InsertCell(std::string hash);

)