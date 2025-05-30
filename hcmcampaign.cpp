#include "hcmcampaign.h"

////////////////////////////////////////////////////////////////////////
/// STUDENT'S ANSWER BEGINS HERE
////////////////////////////////////////////////////////////////////////

////////////////////////////// Class Unit //////////////////////////////
Unit::Unit(int quantity, int weight, const Position pos) {
    this->quantity = quantity;
    this->weight = weight;
    this->pos = pos;
}

Position Unit::getCurrentPosition() const {
    return this->pos;
}


int Unit::getQuantity() const {
    return this->quantity;
}

int Unit::getWeight() const {
    return this->weight;
}

void Unit::setQuantity(int quantity) {
    this->quantity = quantity;
}

void Unit::setWeight(int weight) {
    this->weight = weight;
}

////////////////////////////// Class Vehicle //////////////////////////////
Vehicle::Vehicle(int quantity, int weight, const Position pos, VehicleType vehicleType) : Unit(quantity, weight, pos) {
    this->vehicleType = vehicleType;
}

int Vehicle::getAttackScore() {
    int typeValue = static_cast<int>(this->vehicleType);

    return (typeValue * 304 + this->quantity * this->weight) / 30;
}

string Vehicle::str() const {
    string typeName[7] = {"TANK", "ARTILLERY", "ARMOREDCAR", "APC", "TRUCK", "MORTAR", "ANTIAIRCRAFT"};
    return "Vehicle[vehicleType=" + typeName[this->vehicleType] + ",quantity=" + to_string(this->quantity) + ",weight=" + to_string(this->weight) + ",pos=" + this->pos.str();
}

VehicleType Vehicle::getVehicleType() const {
    return this->vehicleType;
}

////////////////////////////// Class Infantry //////////////////////////////
int Infantry::calPersonalNumber(int number) {
    int sum = 0;

    while (number > 0) {
        sum += number % 10;
        number = number / 10;
    }

    return sum;
}
Infantry::Infantry(int quantity, int weight, const Position pos, InfantryType infantryType) : Unit(quantity, weight, pos) {
    this->infantryType = infantryType;
}

int Infantry::getAttackScore() {
    int typeValue = static_cast<int>(this->infantryType);

    int score = typeValue * 56 + this->quantity + this->weight;

    if (this->infantryType == SPECIALFORCES && this->weight == sqrt(this->weight) * sqrt(this->weight)) {
        score += 75;
        return score;
    }

    int personalNumber = calPersonalNumber(score + 1975);
    if (personalNumber > 7) {
        this->quantity *= 1.2;
    } else if (personalNumber < 3) {
        this->quantity *= 0.9; 
    }

    return typeValue * 56 + this->quantity + this->weight;
}

string Infantry::str() const {
    string typeName[6] = {"SNIPER", "ANTIAIRCRAFTSQUAD", "MORTARSQUAD", "ENGINEER", "SPECIALFORCES", "REGULARINFANTRY"};
    return "Infantry[infantryType=" + typeName[this->infantryType] + ",quantity=" + to_string(this->quantity) + ",weight=" + to_string(this->weight) + ",pos=" + this->pos.str();
}

InfantryType Infantry::getInfantryType() const {
    return this->infantryType;
}

////////////////////////////// Class Army //////////////////////////////
Army::Army(Unit **unitArray, int size, string name, BattleField *battleField) {
    this->LF = 0;
    this->EXP = 0;
    this->name = name;
    this->battleField = battleField;

    for (int i = 0; i < size; i++) {
        const Vehicle *vehicle = dynamic_cast<const Vehicle*>(unitArray[i]);
        const Infantry *infantry = dynamic_cast<const Infantry*>(unitArray[i]);

        if (vehicle) {
            this->LF += unitArray[i]->getAttackScore();
        } else if (infantry) {
            this->EXP += unitArray[i]->getAttackScore();
        }
    }
    
    if (isSpecialNumber(LF + EXP))
        this->unitList = new UnitList(12);
    else
        this->unitList = new UnitList(8);

    for (int i = 0; i < size; i++)
        this->unitList->insert(unitArray[i]);
}

UnitList* Army::getUnitList() const {
    return this->unitList;
}

int Army::getLF() const {
    return this->LF;
}

int Army:: getEXP() const {
    return this->EXP;
}

void Army::setLF(int LF) {
    this->LF = LF;
}

void Army::setEXP(int EXP) {
    this->EXP = EXP;
}

void Army::recalcIndex() {
    this->LF = 0;
    this->EXP = 0;

    UnitNode* current = this->unitList->getHead();
    while (current) {
        Vehicle* vehicle = dynamic_cast<Vehicle*>(current->unit);
        Infantry* infantry = dynamic_cast<Infantry*>(current->unit);

        if (vehicle) {
            this->LF += vehicle->getAttackScore();
        } else {
            this->EXP += infantry->getAttackScore();
        }
        
        current = current->next;
    }
}

////////////////////////////// Class LiberationArmy //////////////////////////////
LiberationArmy::LiberationArmy(Unit** unitArray, int size, string name, BattleField* battleField) : Army(unitArray, size, name, battleField) {}

int LiberationArmy::getNearestFibonacci(int number) {
    if (number <= 1) {
        return 1;
    }

    int fib1 = 1, fib2 = 1;
    while (fib2 < number) {
        int temp = fib1 + fib2;
        fib1 = fib2;
        fib2 = temp;
    }

    return fib2;
}

void LiberationArmy::fight(Army* enemy, bool defense = false) {
    // Attack case
    if (!defense) {
        // Liberation Army got a boost
        this->LF = ceil(this->LF * 1.5);
        this->EXP = ceil(this->EXP * 1.5);

        /*
        Get vectors of vehicles & infantries for handling.
        To reduce calculation overhead, we will create the attack score vector for each list.
        */

        vector<Vehicle*> vehicleUnits;
        vector<Infantry*> infantryUnits;
        vector<int> vehicleScores, infantryScores;

        // Setup to initialize min as the sum of list vehicles/ infantries
        int minVehicleScore = 0;
        int minInfantryScore = 0;

        // Traverse from head to tail
        UnitNode* current = this->unitList->getHead();
        while (current) {
            Vehicle* vehicle = dynamic_cast<Vehicle*>(current->unit);
            Infantry* infantry = dynamic_cast<Infantry*>(current->unit);

            if (vehicle) {
                vehicleUnits.push_back(vehicle);
                int score = vehicle->getAttackScore();
                vehicleScores.push_back(score);
                minVehicleScore += score;
            } else {
                infantryUnits.push_back(infantry);
                int score = infantry->getAttackScore();
                infantryScores.push_back(infantry->getAttackScore());
                minInfantryScore += score;
            }

            current = current->next;
        }

        // Initialization for combination
        vector<Vehicle*> bestVehicleCombination;
        vector<Infantry*> bestInfantryCombination; 

        bool foundVehicleCombination = minVehicleScore >= enemy->getLF(),
             foundInfantryCombination = minInfantryScore >= enemy->getEXP();

        // We only loop and get in case of there exists a combination
        if (foundVehicleCombination) {
            int vehicleCount = vehicleUnits.size();

            // Bitmask technique to get all combination
            for (int mask = 1; mask < (1 << vehicleCount); mask++) {
                vector<Vehicle *> combination;
                int totalScore = 0;

                for (int i = 0; i < vehicleCount; i++) {
                    if (mask & (1 << i)) {
                        combination.push_back(vehicleUnits[i]);
                        totalScore += vehicleScores[i];
                    }
                }

                if (totalScore > enemy->getEXP() && totalScore < minVehicleScore) {
                    minVehicleScore = totalScore;
                    bestVehicleCombination = combination;
                }
            }
        }
        
        if (foundInfantryCombination) {
            int infantryCount = infantryUnits.size();
            for (int mask = 1; mask < (1 << infantryCount); mask++) {
                vector<Infantry*> combination;
                int totalScore = 0;

                for (int i = 0; i < infantryCount; i++) {
                    if (mask & (1 << i)) {
                        combination.push_back(infantryUnits[i]);
                        totalScore += infantryScores[i];
                    }
                }

                if (totalScore > enemy->getEXP() && totalScore < minInfantryScore) {
                    minInfantryScore = totalScore;
                    bestInfantryCombination = combination;
                }
            }
        }        

        bool win = false;
        
        // Condition check for fight result
        switch (foundVehicleCombination << 1 + foundInfantryCombination) {
            // Case found both combination: Will definitely win
            case 3:
                win = true;
                break;
            // Case found Vehicle combination: Check if win
            case 2:
                if (EXP > enemy->getEXP()) {
                    win = true;
                    bestInfantryCombination = infantryUnits;
                }
            // Case found Infantry combination: Check if win
            case 1:
                if (LF > enemy->getLF())
                {
                    win = true;
                    bestVehicleCombination = vehicleUnits;
                }
                break;
            // Default lost, no need to do anything
        }
        
        if (win) {
            // Delete all the combinations, add enemy
            for (Vehicle* unit : bestVehicleCombination)
                this->unitList->deleteVehicle(unit);
            for (Infantry* unit : bestInfantryCombination)
                this->unitList->deleteInfantry(unit);

            /*
                Repurpose enemy's units - 3 cases:

                Case 1: Successfully find the unit in Liberation unitList
                Update quantity, delete the pointer of enemy's unit (pointer = nullptr)

                Case 2: Not find, still have capacity (check the capacity)
                Call insert, if insert return true (means add to Unit* successfully)

                Case 3: Not find and have no capacity (full)
                Call insert - insert return false (means add fail)
                Add unit* to additional vector to put back into enemy later
            */

            // Pop all units from enemy into a temporary vector 
            vector<Unit*> enemyUnits;
            Unit* unit = enemy->getUnitList()->pop_front();
            while (unit) {
                enemyUnits.push_back(unit);
                Unit *unit = enemy->getUnitList()->pop_front();
            }

            // Vector to store units that couldn't be inserted (Case 3)
            /* Some note on case 3 issue
            In UnitList implementation, we always insert vehicle at the end, and infantry at the start
            Since we extract with pop_front, we preserve the order of UnitList
            But if use insert, infantry order will messed up
            For example, we have 0, 1, 2, 3 acting as infantry in UnitList
            When pushing them with insert, they will be inserted in order 0 -> 1 -> 2 -> 3
            Therefore the final result is 3, 2, 1, 0 #BUG
            In order to resolve it, we need to seperate infantries and vehicles
            */
            vector<Unit*> leftoverVehicle, leftoverInfantry;

            // Process each enemy unit
            for (Unit* enemyUnit : enemyUnits) {
                bool unitExists = false;

                Vehicle* enemyVehicle = dynamic_cast<Vehicle*>(enemyUnit);
                Infantry* enemyInfantry = dynamic_cast<Infantry*>(enemyUnit);

                // Case 1: Check if unit exists in current unitList
                // TODO: Update the quantity in Lib Army and delete enemy unit
                if (enemyVehicle) {
                    Unit* unit = this->unitList->getVehicle(enemyVehicle->getVehicleType());
                    if (unit) {
                        unitExists = true;
                        unit->setQuantity(unit->getQuantity() + enemyVehicle->getQuantity());
                        delete enemyVehicle;
                        enemyVehicle = nullptr;
                    }
                }
                else {
                    Unit *unit = this->unitList->getInfantry(enemyInfantry->getInfantryType());
                    if (unit)
                    {
                        unitExists = true;
                        unit->setQuantity(unit->getQuantity() + enemyInfantry->getQuantity());
                        delete enemyInfantry;
                        enemyInfantry = nullptr;
                    }
                }

                // Case 2 & 3: The unit does not exist. Attempting to insert
                if (!unitExists) {
                    if (!this->unitList->insert(enemyUnit)) {
                        // Insert does not success
                        if (enemyVehicle) 
                            leftoverVehicle.push_back(enemyVehicle);
                        else
                            leftoverInfantry.push_back(enemyInfantry);
                    }
                }
            }

            // Resolve leftover units
            // Infantries will be inserted in reversed order
            for (auto it = leftoverInfantry.rbegin(); it != leftoverInfantry.rend(); ++it)
                enemy->getUnitList()->insert(*it);
            // Vehicle just chilling
            for (auto unit : leftoverVehicle)
                enemy->getUnitList()->insert(unit);
        }
        else {
            // Lose - each units lost 10% its weight
            UnitNode* current = this->unitList->getHead();
            while (current)
                current->unit->setWeight(ceil(current->unit->getWeight() * 0.9));

        }

        // Update index - This debunk the fight start effect, so no need to worry
        recalcIndex();

    } else {
    // Defense case

        // Liberation Army has a boost of 1.3
        this->LF = ceil(this->LF * 1.3);
        this->EXP = ceil(this->EXP * 1.3);

        // We loop till the fight end
        bool done = false;
        while (!done) {
            done = true;
            // Case 1: If both LF and EXP of Liberation Army are not lower than enemy's, Liberation Army win - do nothing
            // Case 2: If neither LF nor EXP of Liberation Army is lower than enemy's, need reinforcements - increase to the nearest Fibonacci number
            bool bothLower = (this->LF < enemy->getLF() && this->EXP < enemy->getEXP());

            // Case 3: If either LF or EXP of Liberation Army is lower than enemy's, each unit reduces 10% of its quantity
            bool oneLower = (this->LF < enemy->getLF()) ^ (this->EXP < enemy->getEXP());

            // Reduction of each unit's quantity
            if (oneLower)
            {
                UnitNode *current = this->unitList->getHead();
                while (current)
                {
                    current->unit->setQuantity(ceil(current->unit->getQuantity() * 0.9));

                    if (current->unit->getQuantity() <= 1)
                    {
                        this->unitList->remove(current);
                    }

                    current = current->next;
                }
            }

            // Call reinforcements
            if (bothLower) {
                UnitNode *current = this->unitList->getHead();
                while (current)
                {
                    current->unit->setQuantity(getNearestFibonacci(current->unit->getQuantity()));
                    current = current->next;
                }
                done = false;
            }

            // Update index
            recalcIndex();
        }     
    }
}


string LiberationArmy::str() const {
    return "LiberationArmy[name=" + this->name + ",LF=" + to_string(this->LF) + ",EXP=" + to_string(this->EXP) + ",unitList=" + this->unitList->str() + ",battleField=" + this->battleField->str() + "]";
}

////////////////////////////// Class ARVN //////////////////////////////
ARVN::ARVN(Unit** unitArray, int size, string name, BattleField* battleField) : Army(unitArray, size, name, battleField) {}

void ARVN::fight(Army* fight, bool defense = false) {
    // Attack
    if (!defense) {
        // LF and EXP unchange

        UnitNode *current = this->unitList->getHead();
        while (current)
        {
            current->unit->setQuantity(ceil(current->unit->getQuantity() * 0.9));

            if (current->unit->getQuantity() <= 1)
            {
                this->unitList->remove(current);
            }

            current = current->next;
        }
    } 
    // Defense
    else {
        UnitNode *current = this->unitList->getHead();
        while (current)
        {
            current->unit->setQuantity(ceil(current->unit->getQuantity() * 0.8));

            if (current->unit->getQuantity() <= 1)
            {
                this->unitList->remove(current);
            }

            current = current->next;
        }
    }

    // Recalculate index
    recalcIndex();
}

string ARVN::str() const {
    return "ARVN[name=" + this->name + ",LF=" + to_string(this->LF) + ",EXP=" + to_string(this->EXP) + ",unitList=" + this->unitList->str() + ",battleField=" + this->battleField->str() + "]";
}

////////////////////////////// Class UnitList //////////////////////////////
bool Army::isSpecialNumber(int number) {
    vector<int> power = {
        // Sum of powers of 3
        1, 3, 4, 9, 10, 12, 13, 27, 28, 30, 31, 36, 37, 39, 40,
        81, 82, 84, 85, 90, 91, 93, 94, 108, 109, 111, 112, 117, 118, 120, 121,
        243, 244, 246, 247, 252, 253, 255, 256, 270, 271, 273, 274, 279, 280, 282, 283,
        324, 325, 327, 328, 333, 334, 336, 337, 351, 352, 354, 355, 360, 361, 363, 364,
        729, 730, 732, 733, 738, 739, 741, 742, 756, 757, 759, 760, 765, 766, 768, 769,
        810, 811, 813, 814, 819, 820, 822, 823, 837, 838, 840, 841, 846, 847, 849, 850,
        972, 973, 975, 976, 981, 982, 984, 985, 999, 1000,

        // Sum of powers of 5
        /* 1, */ 5, 6, 25, 26, /* 30, */ /* 31, */ 125, 126, 130, 131, 150, 151, 155, 156,
        625, 626, 630, 631, 650, 651, 655, 656, 750, 751, 755, /* 756, */ 775, 776, 780, 781,

        // Sum of powers of 7
        /* 1, */ 7, 8, 49, 50, 56, 57, 343, 344, 350, /* 351, */ 392, 393, 399, 400};

    for (auto num : power)
        if (num == number)
            return true;

    return false;
}

UnitList::UnitList(int capacity) {
    this->capacity = capacity;

    this->head = nullptr;
    this->vehicleCount = 0;
    this->infantryCount = 0;
}

bool UnitList::insert(Unit *unit) {
    if (vehicleCount + infantryCount >= this->capacity) {
        return false;
    }

    Vehicle* vehicle = dynamic_cast<Vehicle*>(unit);
    Infantry* infantry = dynamic_cast<Infantry*>(unit);

    UnitNode* newNode = new UnitNode;
    newNode->unit = unit;
    newNode->next = nullptr;

    if (vehicle) {
        if (!this->head) {
            this->head = newNode;
        } else {
            UnitNode* current = this->head;
            while (current) {
                current = current->next;
            }
            current->next = newNode;
        }
    } else if (infantry) {
        newNode->next = this->head;
        this->head = newNode;
        this->infantryCount++;
    }

    return true;
}

bool UnitList::isContain(VehicleType vehicleType) {
    UnitNode* current = this->head;

    while (current) {
        Vehicle* vehicle = dynamic_cast<Vehicle*>(current->unit);

        if (vehicle && vehicle->vehicleType == vehicleType) {
            return true;
        }

        current = current->next;
    }

    return false;
}

bool UnitList::isContain(InfantryType infantryType) {
    UnitNode* current = this->head;

    while (current) {
        Infantry* infantry = dynamic_cast<Infantry*>(current->unit);

        if (infantry && infantry->infantryType == infantryType) {
            return true;
        }

        current = current->next;
    }

    return false;
}

Vehicle* UnitList::getVehicle(VehicleType vehicleType) {
    if (!this->head) return nullptr;

    UnitNode* current = this->head;
    while (current) {
        Vehicle* vehicle = dynamic_cast<Vehicle*>(current->unit);
        if (vehicle && vehicle->vehicleType == vehicleType) {
            return vehicle;
        }
        current = current->next;
    }
    return nullptr;
}

Infantry* UnitList::getInfantry(InfantryType infantryType) {
    if (!this->head) return nullptr;

    UnitNode* current = this->head;
    while (current) {
        Infantry* infantry = dynamic_cast<Infantry*>(current->unit);
        if (infantry && infantry->infantryType == infantryType) {
            return infantry;
        }
        current = current->next;
    }
    return nullptr;
}

string UnitList::str() const {
    string output = "UnitList[count_vehicle=" + to_string(this->vehicleCount) + ";count_infantry=" + to_string(this->infantryCount) + ";" + this->head->unit->str();

    UnitNode* current = this->head->next;
    while (current) {
        output += "," + current->unit->str();
        current = current->next;
    }    

    output += "]";
    return output;
}

UnitList::~UnitList() {
    UnitNode* current = this->head;
    while (current) {
        UnitNode* next = current->next;
        delete current;
        current = next;
    }
}

void UnitList::remove(UnitNode* node) {
    if (!node || !this->head) return;

    UnitNode* current = this->head;
    while (current) {
        if (current = node) {
            UnitNode* temp = current;
            current = temp->next;
            delete temp->unit;
            delete temp;
            return;
        }
        current = current->next;
    }

    Vehicle* vehicle = dynamic_cast<Vehicle*>(node->unit);
    Infantry* infantry = dynamic_cast<Infantry*>(node->unit);

    if (vehicle) {
        this->vehicleCount--;
    } else {
        this->infantryCount--;
    }

    delete node->unit;
    delete node;
}

Unit* UnitList::pop_front() {
    if (this->head == nullptr)
        return nullptr;
    UnitNode* head = this->head;
    this->head = head->next;
    Unit* returnUnit = head->unit;
    delete head;
    if (dynamic_cast<Vehicle *>(returnUnit))
        vehicleCount--;
    else    
        infantryCount--;
    return returnUnit;
}

UnitNode* UnitList::getHead() const {
    return this->head;
}

void UnitList::deleteVehicle(Vehicle* vehicle) {
    UnitNode* current = this->head;
    UnitNode* prev = nullptr;

    while (current) {
        if (current->unit == vehicle) {
            // Remove the node from the list
            if (prev) {
                prev->next = current->next;
            } else {
                this->head = current->next;
            }

            // Update vehicleCount
            this->vehicleCount--;
            delete current;
            
            // Unit found and deleted, exit method
            return;
        }
    
        prev = current;
        current = current->next;
    }
}

void UnitList::deleteInfantry(Infantry* infantry) {
    UnitNode* current = this->head;
    UnitNode* prev = nullptr;

    while (current) {
        if (current->unit == infantry) {
            // Remove the node from the list
            if (prev) {
                prev->next = current->next;
            } else {
                this->head = current->next;
            }

            // Update vehicleCount
            this->infantryCount--;
            delete current;
            
            // Unit found and deleted, exit method
            return;
        }
    
        prev = current;
        current = current->next;
    }
}

////////////////////////////// Class Position //////////////////////////////
Position::Position(int r = 0, int c = 0)
{
    this->r = r;
    this->c = c;
}

Position::Position(const string &str_pos)
{
    sscanf(str_pos.c_str(), "(%d,%d)", &this->r, &this->c);
}

int Position::getRow() const
{
    return this->r;
}

int Position::getCol() const
{
    return this->c;
}

void Position::setRow(int r)
{
    this->r = r;
}

void Position::setCol(int c)
{
    this->c = c;
}

string Position::str() const
{
    return "(" + to_string(this->r) + "," + to_string(this->c) + ")";
}

////////////////////////////// Class TerrainElement //////////////////////////////

TerrainElement::TerrainElement(Position pos) {
    this->pos = pos;
}

TerrainElement::~TerrainElement() {}

double TerrainElement::calcDistance(const Position& pos1, const Position& pos2) {
    int dr = abs(pos1.getRow() - pos2.getRow());
    int dc = abs(pos1.getCol() - pos2.getCol());
    return sqrt(dr * dr + dc * dc);
}

Position TerrainElement::getPosition() const {
    return this->pos;
}

////////////////////////////// Class Road //////////////////////////////

Road::Road(Position pos) : TerrainElement(pos) {}

Road::~Road() {}

void Road::getEffect(Army *army) {}

////////////////////////////// Class Mountain //////////////////////////////

Mountain::Mountain(Position pos) : TerrainElement(pos) {}

Mountain::~Mountain() {}

void Mountain::getEffect(Army* army) {
    LiberationArmy *libArmy = dynamic_cast<LiberationArmy *>(army);

    UnitNode *current = army->getUnitList()->getHead();
    while (current)
    {
        Unit *unit = current->unit;
        Vehicle *vehicle = dynamic_cast<Vehicle *>(unit);
        double distance = calcDistance(this->pos, unit->getCurrentPosition());

        if (libArmy) {
            if (distance <= 2.0) {
                if (vehicle)
                    army->setLF(ceil(army->getLF() - unit->getAttackScore() * 0.1));
                else
                    army->setEXP(ceil(army->getEXP() + unit->getAttackScore() * 0.3));
            }
        }
        else {
            if (distance <= 4.0) {
                if (vehicle)
                    army->setLF(ceil(army->getLF() - unit->getAttackScore() * 0.05));
                else
                    army->setEXP(ceil(army->getEXP() + unit->getAttackScore() * 0.2));
            }
        }

        current = current->next;
    }
}

////////////////////////////// Class River //////////////////////////////

River::River(Position pos) : TerrainElement(pos) {}

River::~River() {}

void River::getEffect(Army* army) {
    UnitNode *current = army->getUnitList()->getHead();
    while (current)
    {
        Unit *unit = current->unit;
        Infantry *infantry = dynamic_cast<Infantry *>(unit);
        double distance = calcDistance(this->pos, unit->getCurrentPosition());

        if (distance <= 2.0)
            if (infantry)
                army->setEXP(ceil(army->getEXP() - unit->getAttackScore() * 0.1));

        current = current->next;
    }
}

////////////////////////////// Class Urban //////////////////////////////

Urban::Urban(Position pos) : TerrainElement(pos) {}

Urban::~Urban() {}

void Urban::getEffect(Army *army) {
    LiberationArmy *libArmy = dynamic_cast<LiberationArmy *>(army);

    UnitNode *current = army->getUnitList()->getHead();
    while (current)
    {
        Unit *unit = current->unit;
        Vehicle *vehicle = dynamic_cast<Vehicle *>(unit);
        Infantry *infantry = dynamic_cast<Infantry *>(unit);
        double distance = calcDistance(this->pos, unit->getCurrentPosition());

        if (libArmy)
        {
            if (vehicle && vehicle->getVehicleType() == ARTILLERY && distance <= 2.0)
                army->setLF(army->getLF() - ceil(unit->getAttackScore() * 0.5));
            if (infantry && (infantry->getInfantryType() == SPECIALFORCES || infantry->getInfantryType() == REGULARINFANTRY) && distance <= 5.0)
                army->setLF(army->getEXP() + ceil(2 * unit->getAttackScore() / distance));
        }
        else
        {
            if (infantry && infantry->getInfantryType() == REGULARINFANTRY && distance <= 5.0)
                army->setLF(army->getEXP() + ceil(3 * unit->getAttackScore() / (distance * 2)));
        }

        current = current->next;
    }
}

////////////////////////////// Class Fortification //////////////////////////////

Fortification::Fortification(Position pos) : TerrainElement(pos) {}

Fortification::~Fortification() {}

void Fortification::getEffect(Army *army) {
    LiberationArmy *libArmy = dynamic_cast<LiberationArmy *>(army);

    UnitNode *current = army->getUnitList()->getHead();
    while (current)
    {
        Unit *unit = current->unit;
        Vehicle *vehicle = dynamic_cast<Vehicle *>(unit);
        double distance = calcDistance(this->pos, unit->getCurrentPosition());

        if (libArmy)
        {
            if (distance <= 2.0)
            {
                if (vehicle)
                    army->setLF(ceil(army->getLF() - unit->getAttackScore() * 0.2));
                else
                    army->setEXP(ceil(army->getEXP() - unit->getAttackScore() * 0.2));
            }
        }
        else
        {
            if (distance <= 4.0)
            {
                if (vehicle)
                    army->setLF(ceil(army->getLF() + unit->getAttackScore() * 0.2));
                else
                    army->setEXP(ceil(army->getEXP() + unit->getAttackScore() * 0.2));
            }
        }

        current = current->next;
    }
}

////////////////////////////// Class BattleField //////////////////////////////

string BattleField::str() const {
    return "BattleField[n_rows=" + to_string(this->n_rows) + ",n_cols=" + to_string(this->n_cols);
}

////////////////////////////////////////////////
/// END OF STUDENT'S ANSWER
////////////////////////////////////////////////