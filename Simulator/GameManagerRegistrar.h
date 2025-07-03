#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <cassert>
#include "../common/AbstractGameManager.h"



class GameManagerRegistrar {
    class GameManagerFactoryPair {
        std::string so_name;
        GameManagerFactory gameManagerFactory;
    public:
        GameManagerFactoryPair(const std::string& so_name) : so_name(so_name) {}
        void setGameManagerFactory(GameManagerFactory&& factory) {
            assert(gameManagerFactory == nullptr);
            gameManagerFactory = std::move(factory);
        }
        const std::string& name() const { return so_name; }
        std::unique_ptr<AbstractGameManager> createGameManager(bool verbose) const {
            return gameManagerFactory(verbose);
        }
        bool hasGameManagerFactory() const {
            return gameManagerFactory != nullptr;
        }
    };
    std::vector<GameManagerFactoryPair> gameManagers;
    static GameManagerRegistrar registrar;
public:
    static GameManagerRegistrar& getGameManagerRegistrar();
    void createGameManagerFactoryEntry(const std::string& name) {
        gameManagers.emplace_back(name);
    }
    void addGameManagerFactoryToLastEntry(GameManagerFactory&& factory) {
        gameManagers.back().setGameManagerFactory(std::move(factory));
    }
    struct BadRegistrationException {
        std::string name;
        bool hasName, hasGameManagerFactory;
    };
    void validateLastRegistration() {
        const auto& last = gameManagers.back();
        bool hasName = (last.name() != "");
        if(!hasName || !last.hasGameManagerFactory()) {
            throw BadRegistrationException{
                .name = last.name(),
                .hasName = hasName,
                .hasGameManagerFactory = last.hasGameManagerFactory(),
            };
        }
    }
    void removeLast() {
        gameManagers.pop_back();
    }
    GameManagerFactoryPair& getLast() { // Addition to the provided Skeleton
        return gameManagers.back();
    }
    auto begin() const {
        return gameManagers.begin();
    }
    auto end() const {
        return gameManagers.end();
    }
    std::size_t count() const { return gameManagers.size(); }
    void clear() { gameManagers.clear(); }
};