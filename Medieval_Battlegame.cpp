//
//
//
//
//  How does this work? You can hire an army of different classes, and battle each other
//
#include <iostream>
#include <vector>
#include <string>
using namespace std;
 class Lord;
    // Simulate the behavior of Protector
    class Protector {
        friend std::ostream &operator<<(std::ostream &os, const Protector& rhs);
    public:
        // constructor
        Protector(const std::string& theName, double theStrength);
        
        virtual ~Protector();

        // getters and setters
        const std::string& getName() const;
        double getStrength() const;
        void setStrength(double theStrength);
        void setLord(Lord* nobleptr);
        Lord* getLord() const;
        void hire();
        void fire();
        bool isHired() const;
        // let the Protector runaway
        bool runaway();

        // pure virtual defend method
        virtual void defend() const = 0;

    private:
        // private variables of Protector class
        std::string name;
        double strength;
        Lord* lord;
    };  // class Protector

    class Warrior : public Protector {
    public:
        using Protector::Protector;
        void defend() const override;
    };  // class Warrior

    class Wizard : public Protector {
    public:
        using Protector::Protector;
        void defend() const override;
    };  // class Wizard

    class Archer : public Warrior {
    public:
        using Warrior::Warrior;
        void defend() const final;
    };  // class Archer

    class Swordsman : public Warrior {
    public:
        using Warrior::Warrior;
        void defend() const final;
    };  // class Swordsmen
       class Noble {
        bool is_battle_valid(const Noble &enemy) const;

       
    public:
        // constructor
        Noble(const std::string& theName);

        // getters and setters
        const std::string& getName() const;

        void battle(Noble& enemy);  // Battle between Nobles

        virtual void die() = 0;
        // pure virtual methods:
        //getter for strength of each protector class
        virtual double getStrength() const = 0;
        // if the instance is a Lord, update strength of every Protector
        // if it is a PersonWithStrengthToFight, update itself's strength
         /*
        loops through vector, and substracts ratio from original strength of
        warrior
        doubles as way to kill all warriors input = 0
        */
        virtual void powerReduction(double winner_strength,
                                    double loser_strength) = 0;
        // if the instance is a Lord, call defend() for every Protector
        // if it is a PersonWithStrengthToFight, output "Ugh!"
        virtual void defend() const = 0;

    protected:  // protected getter and setter
        void changeAliveStatus();
        bool isAlive() const;

    private:  // variables of Noble class
        std::string name;
        bool alive;
    };  // class Noble

    class Lord : public Noble {
        friend std::ostream &operator<<(std::ostream &os, const Lord &somelord);
    public:
        Lord(const std::string& theName);  // constructor

        double getStrength() const override;
        size_t Protector_search_system(const Protector &warriorname) const;
        // multiply strength of each protector with the ratio
        void powerReduction(double winner_strength, double loser_strength)
         override;
        void defend() const override;  // call defend() for every Protector
        // hire/fire a protector
        bool hires(Protector& someprotector);
        bool fires(Protector& someprotector);
        void die() override;  // change the status of "alive" and
        //set strength to 0
        // attempt to remove a protector (fire/runaway)
        bool remove(Protector& someprotector);

    private:  // variables of Lord class
        std::vector<Protector*> army;
    };  // class Lord

    class PersonWithStrengthToFight : public Noble {
        friend std::ostream &operator<<(std::ostream &os,
        const PersonWithStrengthToFight &rhs);
    public:
        PersonWithStrengthToFight(const std::string& theName,
         double theStrength);  // constructor

        // getter and setter
        void powerReduction(double winner_strength, double loser_strength) override;
        double getStrength() const final;
        void setStrength(double in);
        void die() override;
        void defend() const final;

    private:  // variables of PersonWithStrengthToFight class
        double strength;
    };  // class PersonWithStrengthToFight
    int main() {
    Lord sam("Sam");
    Archer samantha("Samantha", 200);
    sam.hires(samantha);
    Lord joe("Joe");
    PersonWithStrengthToFight randy("Randolf the Elder", 250);
    Lord janet("Janet");
    Swordsman hardy("TuckTuckTheHardy", 100);
    Swordsman stout("TuckTuckTheStout", 80);
    janet.hires(hardy);
    janet.hires(stout);
    janet.hires(samantha);     // fails because samantha works for sam.
    PersonWithStrengthToFight barclay("Barclay the Bold", 300);
    cout << "\n==========\nNobles: \n"
         << sam << endl
         << randy << endl
         << janet << endl
         << barclay << endl
         << joe << endl
         << "\n==========\n";
    joe.battle(randy);                // joe has no army and dies.
    joe.battle(sam);                // joe is dead
    janet.battle(barclay);          // barclay wins
    Archer pethora("Pethora", 50);
    Archer thora("Thorapleth", 60);
    Wizard merlin("Merlin", 150);
    janet.hires(pethora);          // janet is dead, so can't hire
    sam.hires(thora);              // sam brings in reinforcements
    sam.hires(pethora);
    sam.hires(merlin);
    janet.battle(barclay);       // Silly janet
    sam.battle(barclay);           // Go Sam!
    samantha.runaway();            // Samantha has had enough
    sam.fires(thora);              // These layoffs! Sam fires thora.
    joe.battle(barclay);           // They're both dead already

    cout << "\n==========\nNobles: \n"
         << sam << endl
         << randy << endl
         << janet << endl
         << barclay << endl
         << joe << endl
         << "==========\n";
}
    Protector::Protector(const std::string &theName, double theStrength):
name(theName),strength(theStrength),lord(nullptr){};

// getters and setters
  const std::string& Protector::getName() const{return name;}

  double Protector::getStrength() const{return strength;}

    Lord* Protector::getLord() const { return lord; }

void Protector::setStrength(double theStrength){strength = theStrength;}

void Protector::setLord(Lord *nobleptr){lord = nobleptr;}

// let the Protector runaway
bool Protector::runaway(){
        if (strength == 0) { // fail if warrior is dead
            cout << name << " is already dead!" << endl;
            return false;
        }
        if (!lord) { // fail if lord is a nullptr
            cout << name << " doesn't have a lord!";
            return false;
        }
        string lordname = lord->getName();
        if (lord->remove(*this)) {
            cout << name << " flees in terror, abandoning his lord, "
                 << lordname << endl;
            return true;
        }
        return false; // fail if the warrior is not removed from the army
}
    void Warrior::defend() const {
        cout << getName() << " says: Take that in the name of my lord, "
             << getLord()->getName() << endl;
    }
    bool Protector::isHired() const{
        return lord != nullptr;
    }
    Protector::~Protector(){}
    void Protector::hire(){}
    void Protector::fire(){lord = nullptr;}
    // Wizard Implementation Code
    void Wizard::defend() const { cout << "POOF!" << endl; }

    // Archer Implementation Code
    void Archer::defend() const {
        cout << "TWANG! ";
        Warrior::defend();
    }

    // Archer Implementation Code
    void Swordsman::defend() const {
        cout << "CLANG! ";
        Warrior::defend();
    }
    ostream &operator<<(ostream &os, const Protector &rhs){
        os << rhs.name << " has a strength " << rhs.strength;
        return os;
    }
      Noble::Noble(const std::string& theName): name(theName), alive(true) {}
    bool Noble::is_battle_valid(const Noble& enemy) const{
        {
            if (!alive && !enemy.alive)
            {
                cout << "Oh, NO! They're both dead! Yuck!" << endl;
            }
            else if (alive && !enemy.alive)
            {
                cout << "He's dead, " << name << endl;
            }
            else if (!alive)
            {
                cout << "He's dead, " << enemy.name << endl;
            }
            return alive && enemy.alive;
        }
    }
    void Lord::powerReduction(double winner_strength, double loser_strength) {
            for (Protector* curr_ptr : army) {
                double updatedStrength = curr_ptr->getStrength()*
                        (1-(loser_strength/winner_strength));
                curr_ptr->setStrength(updatedStrength);
            }
      }
    void PersonWithStrengthToFight::powerReduction(double winner_strength,
    double loser_strength) {
        double new_strength = getStrength() - loser_strength;
          setStrength(new_strength);
      }
    size_t Lord::Protector_search_system(const Protector &warriorname) const{
        for (size_t i = 0; i < army.size(); i++)
        {
            if (&warriorname == army[i])
            {
                return i;
            }
        }
        return army.size();
      }
        // getters and setters
        const std::string& Noble::getName() const{return  name;}
        // Bbttle between Nobles
        void Lord::die(){
            for (Protector *w : army)
            {
                w->setStrength(0);
            }
            changeAliveStatus();
      }// change the status of "alive" and set strength to 0
      void PersonWithStrengthToFight::setStrength(double theStrength)
       {strength = theStrength;}
        // pure virtual methods:
        //getter for strength of each protector class
        double Lord::getStrength()const{
            double counter = 0;
            for(const Protector* p:army){
                counter+=p->getStrength();
            }
            return counter;
        }
    void Noble::battle(Noble &enemy)
    {
        cout << name << " battles " << enemy.name << endl;
        defend();
        enemy.defend();
        //calls helper method
        if (is_battle_valid(enemy))
        {
            double selfPower = getStrength();
            double enemyPower = enemy.getStrength();
            double ratio = 0.0;
            if (selfPower > enemyPower)
            {
                powerReduction(selfPower,enemyPower);
                enemy.die();

                cout << name << " defeats " << enemy.name << endl;
            }
            else if (selfPower < enemyPower)
            {
                ratio = selfPower / enemyPower;
                enemy.powerReduction(enemyPower,selfPower);
                die();
                cout << enemy.name << " defeats " << name << endl;
            }
            else
            {
                die();
                enemy.die();
                cout << "Mutual Annihilation: " << name << " and "
                << enemy.name  << " die at each other's hands " << endl;
            }
        }
    }
    double PersonWithStrengthToFight::getStrength()const{return strength;}
        // if the instance is a Lord, update strength of every Protector
        // if it is a PersonWithStrengthToFight, update itself's strength
        // if the instance is a Lord, call defend() for every Protector
        // if it is a PersonWithStrengthToFight, output "Ugh!"
        PersonWithStrengthToFight::PersonWithStrengthToFight(
                const string& theName, double theStrength)
                : Noble(theName), strength(theStrength) {}

    // getter and setter
    void PersonWithStrengthToFight::die(){
         strength = 0;
         changeAliveStatus();
     }
     void Noble::changeAliveStatus(){
          alive = !alive;
      }
    bool Noble::isAlive()const{return alive;}
    Lord::Lord(const std::string& theName): Noble::Noble(theName){}  // constructor
    // multiply strength of each protector with the ratio
    void Lord::defend() const{
        for(Protector* p: army){
            if (isAlive()) {
                p->defend();
            }
        }
    }  // call defend() for every Protector
    // hire/fire a protector
    bool Lord::hires(Protector& someprotector){
        //checks if warrior is hired, if not, fails
        if (!isAlive() || someprotector.isHired())
        {
            cout << getName() << " Failed to hire " << someprotector.getName()
            << endl;
            return false;
        }
        someprotector.hire();
        army.push_back(&someprotector);
        someprotector.setLord(this);
        return true;
    }
    bool Lord::fires(Protector& someprotector){
    
        if (Protector_search_system(someprotector) < army.size() && isAlive())
        {
            cout <<someprotector.getName()
            <<", you don't work for me anymore ! --"<< getName()<< endl;
            remove(someprotector);
            return true;
        }
        return false;
    }
    void PersonWithStrengthToFight::defend() const{
         cout << "UGH!" <<endl;
     }
    // attempt to remove a protector (fire/runaway)
    bool Lord::remove(Protector& someprotector){
        size_t removedWarriorIndex = Protector_search_system(someprotector);
        //if not exist
        if(removedWarriorIndex == army.size()){
            return false;
        }
        //if remove in middle,shift everyone dpwn
        for (size_t move = removedWarriorIndex; move < army.size()-1; move++)
        {
            army[move] = army[move + 1];
        }
        army.pop_back();
        someprotector.fire();
        someprotector.setLord(nullptr);
        return true;
    }
    ostream &operator<<(ostream &os, const Lord &somelord)
    {
        os << somelord.getName() << " has an army of size: "
         << somelord.army.size()<<endl;
        //traverse through vector, dereference and prints out soldier
        for (Protector* p: somelord.army)
        {
            os <<"         " << *p << endl;
        }
        return os;
    }
    ostream &operator<<(ostream &os, const PersonWithStrengthToFight
    &someFighter)
    {
        os << someFighter.getName() << " has a strength of " <<
        someFighter.strength;
        return os;
    }
