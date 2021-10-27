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
    static DownloadedSongsGSM* instance;
    DECLARE_INSTANCE_FIELD(HMUI::ModalView*, modal);
    DECLARE_INSTANCE_FIELD(QuestUI::CustomListTableData*, list);
    //DECLARE_INSTANCE_FIELD(CurvedTextMeshPro*, SongName);
    //DECLARE_INSTANCE_FIELD(CurvedTextMeshPro*, SongAuthor);
    //DECLARE_INSTANCE_FIELD(CurvedTextMeshPro*, SongTime);
    //DECLARE_INSTANCE_FIELD(CurvedTextMeshPro*, SongBpm);

    DECLARE_INSTANCE_METHOD(void, DidActivate, bool firstActivation);
    //DECLARE_INSTANCE_METHOD(void, Awake);
    DECLARE_INSTANCE_METHOD(void, OnEnable);
    DECLARE_INSTANCE_METHOD(void, Refresh);
    //DECLARE_INSTANCE_METHOD(void, NewList);
    public:
    void InsertCell(std::string hash);
    DECLARE_INSTANCE_METHOD(void, CreateCell, System::Threading::Tasks::Task_1<UnityEngine::Sprite*>* coverTask, GlobalNamespace::CustomPreviewBeatmapLevel* level);
    DECLARE_INSTANCE_METHOD(void, Delete);

    //DECLARE_INSTANCE_METHOD(void, DeleteModal, int idx);
    int selectedIdx;
    bool needSongRefresh;
    //bool refreshList;
    public:
    static DownloadedSongsGSM* get_Instance() { return instance; }
    //DECLARE_INSTANCE_METHOD(void, OnDisable);
)