//
// Created by Nick on 5/29/2026.
//

#include <future>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <vector>

#include "../build/vs2026/Release/_deps/sdl-src/include/SDL3/SDL_log.h"
#include "AssetManager.hpp.old"

namespace NK {
template <typename T>
AssetCacheSDL3<T>::AssetCacheSDL3() {}

template <typename T>
AssetCacheSDL3<T>::~AssetCacheSDL3() {
  this->clear();
}

template <typename T>
std::shared_ptr<T> AssetCacheSDL3<T>::getById(std::string_view const& id) {
  // Safe to fetch if no add() or clear() is occurring at the same time
  std::shared_lock<std::shared_mutex> lock(mmtxAssetRWLock);
  auto iter = mmapAssets.find(id);
  if (iter != mmapAssets.end()) {
    return iter->second;
  }
  return nullptr;
}

template <typename T>
void AssetCacheSDL3<T>::add(std::string_view const& id, SDL3Ptr<T> const& pAsset) {
  std::unique_lock<std::shared_mutex> lock(mmtxAssetRWLock);
  mmapAssets[id] = std::move(pAsset);
}

template <typename T>
void AssetCacheSDL3<T>::clear() {
  std::unique_lock<std::shared_mutex> lock(mmtxAssetRWLock);
  mmapAssets.clear();
}

std::shared_ptr<SDL_Texture> AssetManager::getTexture(std::string_view const& szPath) {
  return mcacheTextures.getById(szPath);
}

void AssetManager::process() {
  // Lock & Fetch current state of queue
  std::vector<UploadJob> vecJobs;
  {
    std::lock_guard<std::mutex> lock(mmtxUploadJobQueue);
    vecJobs.swap(mvecUploadJobQueue);
  }

  if (!vecJobs.empty()) [[unlikely]] {
    for (auto& j : vecJobs) {
      SDL_Texture* pTexture = SDL_CreateTextureFromSurface(mpRenderer.get(), j.pSurface.get());
      SDL3Ptr<SDL_Texture> pUniqueTexture(pTexture);
      if (pUniqueTexture)
        mcacheTextures.add(j.id, std::move(pUniqueTexture));
      else
        SDL_LogWarn(SDL_LOG_CATEGORY_VIDEO, "Could not create SDL_Texture object. Error: %s", SDL_GetError());
    }
  }
}

void AssetManager::requestAsyncLoad(std::string const& szPath) {
  // Prevent dupe loading tasks
  if (mcacheTextures.getById(szPath) && mcacheSurfaces.getById(szPath))
    return;

  std::async(std::launch::async, [this, szPath]() {
    SDL_Surface* pRawSurface = SDL_LoadBMP(szPath.c_str());
    if (!pRawSurface)
      return;

    SDL_Ptr<SDL_Surface> pSurface(pRawSurface);

    mcacheSurfaces.add(szPath, std::move(pSurface));

    auto pSharedSurface = mcacheSurfaces.getById(szPath);
    std::lock_guard<std::mutex> lock(mmtxUploadJobQueue);

    SDL_Surface* pDupeSurface = SDL_DuplicateSurface(pSharedSurface.get());
    mvecUploadJobQueue.push_back({szPath, SDL3Ptr<SDL_Surface>(pDupeSurface)});
  });
}

// // Inside your worker thread loop
// thread_local std::unordered_map<std::string, SDL_Ptr<SDL_Surface>> localStagingCache;
//
// void WorkerLoadRoutine(const std::string& path, AssetManager& globalManager) {
//   // Check if this specific worker thread already handled it locally
//   if (localStagingCache.find(path) == localStagingCache.end()) {
//     SDL_Surface* surf = SDL_LoadBMP(path.c_str());
//     localStagingCache[path] = SDL_Ptr<SDL_Surface>(surf);
//
//     // Pass off a copy to the global manager when ready
//     globalManager.RequestAsyncLoad(path);
//   }
// }

}  // namespace NK