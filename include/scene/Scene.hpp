#pragma once

#include <map>
#include <memory>
#include <string>

namespace Game {
    class Scene {
        public:
            Scene();
            ~Scene();
        public:
            std::unique_ptr<void>& getObject(std::string const& szName) const;
        private:
            std::map<std::string, std::unique_ptr<void>> mmapObjects;
    };
}