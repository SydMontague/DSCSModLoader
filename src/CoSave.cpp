#include "CoSave.h"

#include "dscs/GameInterface.h"
#include "modloader/utils.h"

#include <filesystem>
#include <format>

struct CoSaveContainer
{
    uint32_t size;
    uint32_t offset;
    char name[24];
};

struct CoSave
{
    uint32_t magic; // 'DSCS'
    uint32_t version;
    uint32_t containerCount;
    uint32_t reserved;
    CoSaveContainer container[0];
};

void CoSaveImpl::addCoSaveHook(std::string name, CoSaveReadCallback read, CoSaveWriteCallback write)
{
    coSaveCallbacks.emplace(name, std::make_pair(read, write));
}

void CoSaveImpl::createCoSave()
{
    rawContainer.clear();

    for (auto& entry : coSaveCallbacks)
        rawContainer[entry.first] = entry.second.second();
}

void CoSaveImpl::loadCoSave()
{
    for (auto& entry : coSaveCallbacks)
        entry.second.first(rawContainer[entry.first]);
}

bool CoSaveImpl::readCoSave()
{
    auto gameSaveData = dscs::getGameSaveData();
    auto saveSlot     = gameSaveData->loadSlot;
    char saveDirPath[208]{};
    mediavision::vfs::ReadHandle handle;

    rawContainer.clear();
    dscs::getSaveDirPath(saveDirPath);
    const std::string gameFile = std::format("{}cosave_{:0>4}.bin", saveDirPath, saveSlot);

    if (!std::filesystem::exists(gameFile)) return true;
    if (mediavision::vfs::readFile(gameFile.c_str(), &handle) == 0) return false;
    if (handle.size == 0) return true;

    CoSave* header = reinterpret_cast<CoSave*>(handle.buffer);

    if (header->magic != 'DSCS') return false;

    for (int32_t i = 0; i < header->containerCount; i++)
    {
        CoSaveContainer container = header->container[i];
        std::string str(container.name);

        std::vector<uint8_t> data;
        std::copy_n(handle.buffer + container.offset, container.size, std::back_inserter(data));
        rawContainer[str] = data;
    }

    return true;
}

bool CoSaveImpl::writeCoSave()
{
    auto gameSaveData = dscs::getGameSaveData();
    auto saveSlot     = gameSaveData->saveSlot;
    char saveDirPath[208]{};

    dscs::createSaveDir();
    dscs::getSaveDirPath(saveDirPath);
    auto gameFile = std::format("{}cosave_{:04}.bin", saveDirPath, saveSlot);

    mediavision::vfs::deleteFile(gameFile.c_str());

    std::vector<uint8_t> data;
    std::vector<uint8_t> containerData;
    CoSave save;
    save.magic          = 'DSCS';
    save.version        = 1;
    save.containerCount = rawContainer.size();
    save.reserved       = 0;

    std::size_t dataStart = sizeof(save) + save.containerCount * sizeof(CoSaveContainer);

    std::copy(reinterpret_cast<uint8_t*>(&save),
              reinterpret_cast<uint8_t*>(&save) + sizeof(save),
              std::back_inserter(data));

    for (auto& container : rawContainer)
    {
        CoSaveContainer tmp{};

        std::copy_n(container.first.begin(),
                    std::min(container.first.size(), sizeof(tmp.name) - 1),
                    std::begin(tmp.name));
        tmp.name[sizeof(tmp.name) - 1] = '\0';

        tmp.size   = container.second.size();
        tmp.offset = dataStart;

        std::copy_n(reinterpret_cast<uint8_t*>(&tmp), sizeof(tmp), std::back_inserter(data));

        std::copy(container.second.begin(), container.second.end(), std::back_inserter(containerData));

        auto containerSize = containerData.size() + ((0x10 - (containerData.size() % 0x10)) % 0x10);
        containerData.resize(containerSize);

        dataStart += tmp.size;
        dataStart += ((0x10 - (dataStart % 0x10)) % 0x10);
    }

    std::copy(containerData.begin(), containerData.end(), std::back_inserter(data));

    if (mediavision::vfs::writeFile(gameFile.c_str(), data.data(), data.size()) == 0) return false;

    return true;
}

void CoSaveImpl::readSaveFileHook()
{
    auto gameSaveData = dscs::getGameSaveData();

    if (readCoSave())
        readSaveFile();
    else
        gameSaveData->loadResponseCode = 3;

    gameSaveData->loadThreadActive = false;
    dscs::endThread(0);
}

void CoSaveImpl::writeSaveFileHook()
{
    auto gameSaveData = dscs::getGameSaveData();

    if (writeCoSave())
        writeSaveFile();
    else
        gameSaveData->saveResponseCode = 5;

    gameSaveData->saveThreadActive = false;
    dscs::endThread(0);
}

void CoSaveImpl::loadSaveHook(void* empty, dscs::Savegame& save)
{
    loadSave(empty, save);
    loadCoSave();
}

void CoSaveImpl::createSaveHook(void* empty, dscs::Savegame& save)
{
    createSave(empty, save);
    createCoSave();
}

void CoSaveImpl::readSaveFile()
{
    auto gameSaveData = dscs::getGameSaveData();
    auto saveSlot     = gameSaveData->loadSlot;
    char saveDirPath[208]{};
    mediavision::vfs::ReadHandle handle;

    dscs::getSaveDirPath(saveDirPath);
    auto gameFile = std::format("{}{:04}.bin", saveDirPath, saveSlot);
    auto result   = mediavision::vfs::readFile(gameFile.c_str(), &handle);

    if (result != 0)
    {
        dscs::decryptSaveFile(handle.buffer, handle.size, 0x6e6f6d696c694c40); // @Lilimon
        memcpy(gameSaveData->loadSave, handle.buffer, handle.size);
        gameSaveData->loadSize         = handle.size;
        gameSaveData->loadResponseCode = 2;
    }
    else
        gameSaveData->loadResponseCode = 3;
}

void CoSaveImpl::writeSaveFile()
{
    struct SlotSaveData
    {
        int32_t data[194];
        time_t time;
    };

    auto gameSaveData = dscs::getGameSaveData();
    auto saveSlot     = gameSaveData->saveSlot;
    char saveDirPath[208]{};
    SlotSaveData slotData{};

    dscs::createSaveDir();
    dscs::getSaveDirPath(saveDirPath);
    auto gameFile = std::format("{}{:04}.bin", saveDirPath, saveSlot);
    auto slotFile = std::format("{}slot_{:04}.bin", saveDirPath, saveSlot);

    // slot file
    mediavision::vfs::deleteFile(slotFile.c_str());
    std::copy(std::begin(gameSaveData->slotData), std::end(gameSaveData->slotData), std::begin(slotData.data));
    slotData.time = _time64(NULL);
    dscs::encryptSaveFile(&slotData, sizeof(slotData), 0x6e6f6d6f6b6f5440); // @Tokomon
    auto slotResult = mediavision::vfs::writeFile(slotFile.c_str(), &slotData, sizeof(slotData));

    if (slotResult == 0)
        gameSaveData->saveResponseCode = 5;
    else
    {
        // game file
        dscs::Savegame* gameFileBuffer = new dscs::Savegame();
        auto size                      = gameSaveData->saveSaveSize;

        mediavision::vfs::deleteFile(gameFile.c_str());
        *gameFileBuffer = *gameSaveData->saveSave;
        dscs::encryptSaveFile(gameFileBuffer, size, 0x6e6f6d696c694c40); // @Lilimon
        auto writeGameResult = mediavision::vfs::writeFile(gameFile.c_str(), gameFileBuffer, size);

        gameSaveData->saveResponseCode = 2;
        if (writeGameResult == 0)
        {
            mediavision::vfs::deleteFile(slotFile.c_str());
            gameSaveData->saveResponseCode = 5;
        }

        delete gameFileBuffer;
    }
}

void CoSaveImpl::loadStorySave(dscs::StorySave& save, bool isHM)
{
    auto context   = dscs::getGameContext();
    auto digimon   = isHM ? context->digimonHM : context->digimonCS;
    auto inventory = isHM ? context->inventoryHM : context->inventoryCS;
    auto quests    = isHM ? context->questsHM : context->questsCS;
    auto digiline  = isHM ? context->digilineHM : context->digilineCS;
    auto player    = isHM ? context->playerHM : context->playerCS;
    auto digiFarm  = isHM ? context->digiFarmHM : context->digiFarmCS;

    for (auto& scanEntry : save.scanData)
        if (scanEntry.digimonId != 0) dscs::setScanData(digimon, scanEntry.digimonId, scanEntry.scanrate);

    for (int32_t i = 0; i < 300; i++)
        *digimon->bank[i] = save.bank[i];
    digimon->bankSize = save.bankSize;
    for (int32_t i = 0; i < 5; i++)
        for (int32_t j = 0; j < 10; j++)
            *digimon->farm[i][j] = save.farm[i][j];

    for (int32_t i = 0; i < 11; i++)
        *digimon->party[i] = save.party[i];

    for (int32_t i = 0; i < 2; i++)
        *digimon->guestDigimon[i] = save.guest[i];

    digiFarm->data = save.digiFarm;

    for (int32_t i = 0; i < 2000; i++)
        *inventory->bag[i] = save.inventoryBag[i];
    for (int32_t i = 0; i < 100; i++)
        *inventory->unk1[i] = save.inventoryUnk1[i];
    for (int32_t i = 0; i < 100; i++)
        *inventory->unk2[i] = save.inventoryUnk2[i];

    player->data = save.player;

    for (int32_t i = 0; i < 30; i++)
        *player->learnedSkills[i] = save.hackingSkills[i];

    for (int32_t i = 0; i < 350; i++)
        if (save.questUnk1[i].id != 0) *quests->unk1.at(save.questUnk1[i].id) = save.questUnk1[i];
    for (int32_t i = 0; i < 350; i++)
        if (save.questUnk2[i].id != 0) *quests->unk2.at(save.questUnk2[i].id) = save.questUnk2[i];
    for (int32_t i = 0; i < 350; i++)
        if (save.questUnk3[i].id != 0) *quests->unk3.at(save.questUnk3[i].id) = save.questUnk3[i];
    quests->unk4 = save.questUnk4;

    for (int32_t i = 0; i < 30; i++)
        *digiline->unk1[i] = save.digiline1[i];
    for (int32_t i = 0; i < 30; i++)
        *digiline->unk2[i] = save.digiline2[i];
    for (int32_t i = 0; i < 30; i++)
        *digiline->unk3[i] = save.digiline3[i];

    for (int32_t i = 0; i < 90; i++) {
        if (save.digilineUnk[i] == 0)
            continue;

        digiline->field4_0x50.push_back(save.digilineUnk[i]);
    }
    for (int32_t i = 0; i < 90; i++)
        *digiline->field5_0x68[i] = save.digiline4[i];
}

void CoSaveImpl::loadSave(void* empty, dscs::Savegame& save)
{
    auto context  = dscs::getGameContext();
    auto seenData = dscs::getSeenData();

    std::copy(std::begin(save.flags), std::end(save.flags), std::begin(context->flags->flags));
    for (uint32_t i = 0; i < 400; i++)
    {
        const auto entryId = save.seenData[i].entryId;
        if (seenData->contains(entryId)) *seenData->at(entryId) = save.seenData[i];
    }
    std::copy(std::begin(save.work), std::end(save.work), std::begin(context->work->flags));

    context->settings->data  = save.settings;
    context->stats->data     = save.stats;
    context->battleBox->data = save.battleBoxData;

    for (int32_t i = 0; i < 6; i++)
    {
        auto& saveBox    = save.battleBoxes[i];
        auto& contextBox = context->battleBox->boxes[i];

        contextBox.field0_0x0 = saveBox.field0_0x0;
        std::copy(std::begin(saveBox.name), std::end(saveBox.name), std::begin(contextBox.name));
        std::copy_n(std::begin(saveBox.party), 11, std::begin(contextBox.party));
    }

    loadStorySave(save.saveCS, false);
    loadStorySave(save.saveHM, true);

    auto sound           = dscs::getSoundStruct();
    sound->field78_0x138 = true;
    std::for_each(sound->field3_0x28.begin(), sound->field3_0x28.end(), [](auto& val) { val.field8_0x3c = true; });
}

void CoSaveImpl::createStorySave(dscs::StorySave& save, bool isHM)
{
    auto context   = dscs::getGameContext();
    auto digimon   = isHM ? context->digimonHM : context->digimonCS;
    auto inventory = isHM ? context->inventoryHM : context->inventoryCS;
    auto quests    = isHM ? context->questsHM : context->questsCS;
    auto digiline  = isHM ? context->digilineHM : context->digilineCS;
    auto player    = isHM ? context->playerHM : context->playerCS;
    auto digiFarm  = isHM ? context->digiFarmHM : context->digiFarmCS;

    copy_max(digimon->scanData, std::begin(save.scanData), 400, [&](auto& val) { return *val->second; });

    copy_max(digimon->bank, std::begin(save.bank), 300, [&](auto& val) { return *val; });
    save.bankSize = digimon->bankSize;
    for (int32_t i = 0; i < 5; i++)
        copy_max(digimon->farm[i], std::begin(save.farm[i]), 10, [&](auto& val) { return *val; });
    copy_max(digimon->party, std::begin(save.party), 11, [&](auto& val) { return *val; });
    copy_max(digimon->guestDigimon, std::begin(save.guest), 2, [&](auto& val) { return *val; });

    save.digiFarm = digiFarm->data;

    copy_max(inventory->bag, std::begin(save.inventoryBag), 2000, [&](auto& val) { return **val; });
    copy_max(inventory->unk1, std::begin(save.inventoryUnk1), 100, [&](auto& val) { return **val; });
    copy_max(inventory->unk2, std::begin(save.inventoryUnk2), 100, [&](auto& val) { return **val; });

    save.player = player->data;
    copy_max(player->learnedSkills, save.hackingSkills, 30, [&](auto& val) { return **val; });

    copy_max(quests->unk1, std::begin(save.questUnk1), 350, [&](auto& val) { return *val->second; });
    copy_max(quests->unk2, std::begin(save.questUnk2), 350, [&](auto& val) { return *val->second; });
    copy_max(quests->unk3, std::begin(save.questUnk3), 350, [&](auto& val) { return *val->second; });
    save.questUnk4 = quests->unk4;

    copy_max(digiline->unk1, std::begin(save.digiline1), 30, [&](auto& val) { return **val; });
    copy_max(digiline->unk2, std::begin(save.digiline2), 30, [&](auto& val) { return **val; });
    copy_max(digiline->unk3, std::begin(save.digiline3), 30, [&](auto& val) { return **val; });
    std::copy(digiline->field4_0x50.begin(), digiline->field4_0x50.end(), std::begin(save.digilineUnk));
    copy_max(digiline->field5_0x68, std::begin(save.digiline4), 90, [&](auto& val) { return **val; });
}

void CoSaveImpl::createSave(void* empty, dscs::Savegame& save)
{
    auto context  = dscs::getGameContext();
    auto seenData = dscs::getSeenData();

    std::memset(&save, 0, sizeof(save));
    save.version = 0x13;
    std::copy(std::begin(context->flags->flags), std::end(context->flags->flags), std::begin(save.flags));
    copy_max(seenData, std::begin(save.seenData), 400, [&](auto& val) { return *val->second; });
    std::copy(std::begin(context->work->flags), std::end(context->work->flags), std::begin(save.work));
    save.settings      = context->settings->data;
    save.stats         = context->stats->data;
    save.battleBoxData = context->battleBox->data;

    for (int32_t i = 0; i < 6; i++)
    {
        auto& saveBox    = save.battleBoxes[i];
        auto& contextBox = context->battleBox->boxes[i];

        saveBox.field0_0x0 = contextBox.field0_0x0;
        std::copy(std::begin(contextBox.name), std::end(contextBox.name), std::begin(saveBox.name));
        std::copy_n(std::begin(contextBox.party), 11, std::begin(saveBox.party));
    }

    createStorySave(save.saveCS, false);
    createStorySave(save.saveHM, true);
}