/*  
*   DownloadedSongsGameplaySetupMenu
*   Copyright (c) EnderdracheLP
*/

#include "main.hpp"
#include "UI/DownloadedSongsGSM.hpp"

#include "questui/shared/BeatSaberUI.hpp"
#include "songloader/shared/API.hpp"

//#include "TMPro/TextAlignmentOptions.hpp"
#include "HMUI/ModalView.hpp"
#include "System/Threading/Tasks/Task.hpp"
#include "System/Action_1.hpp"
using namespace GlobalNamespace;
using namespace QuestUI;
using namespace RuntimeSongLoader::API;
using namespace HMUI;


DEFINE_TYPE(MultiQuestensions::UI, DownloadedSongsGSM);

namespace MultiQuestensions::UI {
    bool cellIsSelected = false;
    DownloadedSongsGSM* DownloadedSongsGSM::instance;

    void DownloadedSongsGSM::CreateCell(System::Threading::Tasks::Task_1<UnityEngine::Sprite*>* coverTask, CustomPreviewBeatmapLevel* level) {
        getLogger().debug("CreateCell");
        UnityEngine::Sprite* cover = coverTask->get_ResultOnSuccess();
        if (cover) {
            // "<size=80%><noparse>" + map.GetMetadata().GetSongAuthorName() + "</noparse>" + " <size=90%>[<color=#67c16f><noparse>" + map.GetMetadata().GetLevelAuthorName() + "</noparse></color>]"
            list->data.emplace_back(CustomListTableData::CustomCellInfo{
                to_utf8(csstrtostr(level->get_songName())),
                to_utf8(csstrtostr(level->get_songAuthorName())) + " [" + to_utf8(csstrtostr(level->get_levelAuthorName())) + "]",
                cover
                });
        }
        else {
            list->data.emplace_back(CustomListTableData::CustomCellInfo{
            to_utf8(csstrtostr(level->get_songName())),
            to_utf8(csstrtostr(level->get_songAuthorName())) + " [" + to_utf8(csstrtostr(level->get_levelAuthorName())) + "]",
            level->get_defaultCoverImage()
                });
        }
        //for (int i = 0; i < list->tableView->visibleCells->get_Count(); i++) {
        //    HMUI::TableCell* cell = list->tableView->visibleCells->get_Item(i);
        //    cell->get_transform()->Find(il2cpp_utils::newcsstr("SongAuthor"))->GetComponent<CurvedTextMeshPro*>()->set_richText(true);
        //}
        //if (refreshList) list->tableView->ReloadData();
        list->tableView->RefreshCellsContent();
        getLogger().debug("CreateCell Finished");
    }

    // TODO: Add index check, check if index is out of bounds
    void DownloadedSongsGSM::Delete() {
        needSongRefresh = false;
        auto level = GetLevelByHash(DownloadedSongIds.at(selectedIdx));
        if (level.has_value()) {
            std::string songPath = to_utf8(csstrtostr(level.value()->customLevelPath));
            getLogger().info("Deleting Song: %s", songPath.c_str());
            DeleteSong(songPath, [&] {
                if (needSongRefresh) {
                    RefreshSongs(false);
                }
                });
        }
        needSongRefresh = true;
        DownloadedSongIds.erase(DownloadedSongIds.begin() + selectedIdx);
        list->tableView->ClearSelection();
        list->data.erase(list->data.begin() + selectedIdx);
        //list->tableView->RefreshCellsContent();
        Refresh();
        modal->Hide(true, nullptr);
    }

    // TODO: Add keep all and delete all option
    void DownloadedSongsGSM::DidActivate(bool firstActivation) {
        if (firstActivation) {
            instance = this;
            //BeatSaberUI::CreateText(get_transform(), "Test")->set_alignment(TMPro::TextAlignmentOptions::Center);

            modal = BeatSaberUI::CreateModal(get_transform(), { 55, 25 }, [this](HMUI::ModalView* self) {
                list->tableView->ClearSelection();
                });
            //QuestUI::BeatSaberUI::CreateText(modal->get_transform(), "Do you want to delete this song?");

            //QuestUI::BeatSaberUI::CreateUIButton(modal->get_transform(), "Delete", Delete);

            auto wrapper = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(modal->get_transform());
            auto container = QuestUI::BeatSaberUI::CreateVerticalLayoutGroup(wrapper->get_transform());
            container->set_childAlignment(UnityEngine::TextAnchor::MiddleCenter);
            QuestUI::BeatSaberUI::CreateText(container->get_transform(), "Do you want to delete this song?")->set_alignment(TMPro::TextAlignmentOptions::Center);

            //objectName = CreateText(container->get_transform(), "PLACEHOLDER");
            //objectName->set_alignment(TMPro::TextAlignmentOptions::Center);

            auto horizon = QuestUI::BeatSaberUI::CreateHorizontalLayoutGroup(container->get_transform());

            QuestUI::BeatSaberUI::CreateUIButton(horizon->get_transform(), "<color=#ff0000>Delete</color>", [this]() -> void {
                Delete();
                cellIsSelected = false;
                });

            QuestUI::BeatSaberUI::CreateUIButton(horizon->get_transform(), "<color=#00ff00>Keep</color>", [this]() -> void {
                DownloadedSongIds.erase(DownloadedSongIds.begin() + selectedIdx);
                list->tableView->ClearSelection();
                list->data.erase(list->data.begin() + selectedIdx);
                Refresh();
                modal->Hide(true, nullptr);
                cellIsSelected = false;
                });

            list = BeatSaberUI::CreateScrollableList(get_transform(), { 80, 60 }, [this](int idx) {
                getLogger().debug("Cell with idx %d clicked", idx);
                cellIsSelected = true;
                selectedIdx = idx;
                modal->Show(true, true, nullptr);
                });

            //NewList();
        }
        getLogger().debug("DownloadedSongsGSM::DidActivate");

        //for (int i = 0; i < list->tableView->visibleCells->get_Count(); i++) {
        //    HMUI::TableCell* cell = list->tableView->visibleCells->get_Item(i);
        //    //SongName = cell->get_transform()->Find(il2cpp_utils::newcsstr("SongName"))->GetComponent<CurvedTextMeshPro*>();
        //    SongAuthor = cell->get_transform()->Find(il2cpp_utils::newcsstr("SongAuthor"))->GetComponent<CurvedTextMeshPro*>();
        //    //ImageView* FavoritesIcon = cell->get_transform()->Find(il2cpp_utils::newcsstr("FavoritesIcon"))->GetComponent<ImageView*>();
        //    SongTime = cell->get_transform()->Find(il2cpp_utils::newcsstr("SongTime"))->GetComponent<CurvedTextMeshPro*>();
        //    SongBpm = cell->get_transform()->Find(il2cpp_utils::newcsstr("SongBpm"))->GetComponent<CurvedTextMeshPro*>();
        //    ImageView* BpmIcon = cell->get_transform()->Find(il2cpp_utils::newcsstr("BpmIcon"))->GetComponent<ImageView*>();

        //    SongAuthor->set_richText(true);

        //    //FavoritesIcon->get_gameObject()->SetActive(false);
        //    SongTime->get_gameObject()->SetActive(true);
        //    SongBpm->get_gameObject()->SetActive(true);
        //    BpmIcon->get_gameObject()->SetActive(false);
        //}

        Refresh();
    }

    void DownloadedSongsGSM::InsertCell(std::string hash) {
        std::optional<CustomPreviewBeatmapLevel*> levelOpt = GetLevelByHash(hash);
        if (levelOpt.has_value()) {
            getLogger().info("Song with Hash '%s' added to list", hash.c_str());
            CustomPreviewBeatmapLevel* level = levelOpt.value();
            System::Threading::Tasks::Task_1<UnityEngine::Sprite*>* coverTask = level->GetCoverImageAsync(System::Threading::CancellationToken::get_None());
            auto action = il2cpp_utils::MakeDelegate<System::Action_1<System::Threading::Tasks::Task*>*>(classof(System::Action_1<System::Threading::Tasks::Task*>*), (std::function<void()>)[coverTask, this, level] {
                CreateCell(coverTask, level/*, list->NumberOfCells() + 1*/);
                }
            );
            reinterpret_cast<System::Threading::Tasks::Task*>(coverTask)->ContinueWith(action);
        }
        else {
            getLogger().error("Song with Hash '%s' not found, was it already deleted?", hash.c_str());
        }
    }

    void DownloadedSongsGSM::Refresh() {
        list->tableView->ReloadData();
        list->tableView->RefreshCellsContent();
    }


    //void DownloadedSongsGSM::NewList() {
    //    //refreshList = false;
    //    list->data.clear();
    //    if (list->NumberOfCells() > 0) list->tableView->DeleteCells(0, list->NumberOfCells());
    //    for (std::string hash : DownloadedSongIds) {
    //        getLogger().debug("Song with Hash '%s'", hash.c_str());
    //        std::optional<CustomPreviewBeatmapLevel*> levelOpt = GetLevelByHash(hash);
    //        if (levelOpt.has_value()) {
    //            CustomPreviewBeatmapLevel* level = levelOpt.value();
    //            System::Threading::Tasks::Task_1<UnityEngine::Sprite*>* coverTask = level->GetCoverImageAsync(System::Threading::CancellationToken::get_None());
    //            auto action = il2cpp_utils::MakeDelegate<System::Action_1<System::Threading::Tasks::Task*>*>(classof(System::Action_1<System::Threading::Tasks::Task*>*), (std::function<void()>)[coverTask, this, level] {
    //                CreateCell(coverTask, level);
    //                }
    //            );
    //            reinterpret_cast<System::Threading::Tasks::Task*>(coverTask)->ContinueWith(action);
    //        }
    //    }
    //    //refreshList = true;
    //}

    //void DownloadedSongsGSM::Awake() {
    //    QuestUI::BeatSaberUI::CreateText(get_transform(), "Test");
    //}

    void DownloadedSongsGSM::OnEnable() {
        if (list && list->tableView) {
            if (cellIsSelected) list->tableView->ClearSelection();
            list->tableView->ReloadData();
            list->tableView->RefreshCellsContent();
        }
    }

    //void DownloadedSongsGSM::OnDisable() {

    //}
}