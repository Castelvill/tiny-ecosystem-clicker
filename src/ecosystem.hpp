#include "entities.hpp"

class Ecosystem: public EcosystemLite{
    vector<Entity> sand;
    vector<Ostracod> ostracods;
public:
    void init();

    void spawnAlgae();
    void updateAlgaes();

    void spawnSand();
    void updateSand();

    void spawnOstracods();
    void updateOstracods();

    void update();
    void draw() const;
};