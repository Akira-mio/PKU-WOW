#include<iostream>
#include<cstring>
#include<algorithm>
#include<vector>
#define MAXINT 0x7fffffff
using namespace std;
//职业为dragon0,ninja1,iceman2,lion3,wolf4
int warrior_life[5]={0};
int warrior_attack[5]={0};
const int red_cycle[5]={2,3,4,1,0};//红方造武士循环
const int blue_cycle[5]={3,0,1,2,4};//蓝方造武士循环
const char warrior_name[5][10]={"dragon","ninja","iceman","lion","wolf"};
//武器类型为sword0,bomb1,arrow2
const char weapon_name[3][10]={"sword","bomb","arrow"};

int city_number=0;
int lion_loyalty_decline=0;
bool war_end;
int arrow_attack=0;
int blue_people;
int red_people;
class city;
class weapon;
city* map[30];

int hour=0;


//武士类
class warrior{
public:
    int color;//武士颜色,0红，1蓝
    int number;//武士编号
    int life;//武士生命
    int attack;//武士攻击力
    int job;//武士职业
    weapon* my_sword;
    weapon* my_arrow;
    weapon* my_bomb;
    
    
    int place;
    //int loyalty=-1;//武士忠诚度
    warrior(int new_color,int new_number,int new_job);
    warrior(warrior &p);
    void get_weapon(int weapon_number);//被造出时获得武器

    void hit(warrior* defender);
    void hit_back(warrior* attacker);

    void use_bomb(warrior* target);
    void use_arrow(warrior* target);
    //void sort_weapon_beforesteal();//被偷，被缴获前排武器顺序
    //void sort_weapon_beforebattle();//打架前排武器顺序
    virtual void loyalty_decline(bool ifwin){};//lion忠诚减损
    virtual void record_life(){};//lion记录战斗前生命
    virtual void lion_givelife(warrior* target){};//lion死后给对手生命
    virtual void life_decline(){};//iceman生命减损
    virtual void steal(warrior* target){};//wolf偷武器
    virtual void dragon_morale_change(bool ifwin){};//dragon士气增减
    virtual void yell(int city_number){};//dragon欢呼
    virtual bool run_success(){return false;}//lion是否成功逃跑
    
    //void use_weapon(warrior* defender);//武士拿武器攻击
    //bool stop();//武士这一轮攻击完毕（武器用完或不停用sword刮痧）
    //void victory_get_weapon(warrior* target);//胜利时缴获武器
    void get_life(){};
    void report();//报告武器和生命
};

//武器类
class weapon{
public:
    int categlory;
    int attack;
    int life;
    weapon(){};
    weapon(int new_categlory,int new_attack){
        categlory=new_categlory;
        attack=new_attack;
    }

};
class sword:public weapon{
public:
    sword(){};
    sword(int new_categlory,int new_attack):weapon(new_categlory,new_attack){
        life=MAXINT;
        if (new_attack==0) life=0;
    }
};
class bomb:public weapon{
public:
    bomb(){};
    bomb(int new_categlory):weapon(new_categlory,9999999){
        life=1;
    }

};
class arrow:public weapon{
public:
    arrow(){};
    arrow(int new_categlory):weapon(new_categlory,arrow_attack){
        life=3;
    }
};



warrior::warrior(int new_color,int new_number,int new_job){
    color=new_color;
    number=new_number;
    job=new_job;
    my_sword=nullptr;
    my_bomb=nullptr;
    my_arrow=nullptr;
    if (color==0) place=0;
        else place=city_number+1;
    if (color==0) printf("%03d:00 red %s %d born\n",hour,warrior_name[job],number);
    else printf("%03d:00 blue %s %d born\n",hour,warrior_name[job],number);
}
warrior::warrior(warrior &p){
    color=p.color;
    number=p.number;
    life=p.life;
    attack=p.attack;
    job=p.job;
    my_sword=nullptr;
    if (p.my_sword!=nullptr) {
        my_sword=new sword;
        *my_sword=*(p.my_sword);
    }
    my_arrow=nullptr;
    if (p.my_arrow!=nullptr) {
        my_arrow=new arrow;
        *my_arrow=*(p.my_arrow);
    }
    my_bomb=nullptr;
    if (p.my_bomb!=nullptr) {
        my_bomb=new bomb;
        *my_bomb=*(p.my_bomb);
    }
}
void warrior::get_weapon(int weapon_number){
    switch (weapon_number){
        case 0:my_sword=new sword(0,attack/5);break;
        case 1:my_bomb=new bomb(1);break;
        case 2:my_arrow=new arrow(2);break;
    }
}

void warrior::hit(warrior* defender){
    defender->life-=attack;
    if (my_sword!=nullptr) {
        defender->life-=my_sword->attack;
        my_sword->attack=my_sword->attack*4/5;
        if (my_sword->attack==0) my_sword=nullptr;
    }
}
void warrior::hit_back(warrior* attacker){
    if (attacker->job==1) return;//ninja不反击
    life-=attacker->attack/2;
    if (attacker->my_sword!=nullptr) {
        life-=attacker->my_sword->attack;
        attacker->my_sword->attack=attacker->my_sword->attack*4/5;
        if (attacker->my_sword->attack==0) attacker->my_sword=nullptr;
    }
}

void warrior::use_bomb(warrior* target){
    life=0;
    target->life=0;
    if (color==0) printf("%03d:38 red %s %d used a bomb and killed blue %s %d\n",hour,warrior_name[job],number,warrior_name[target->job],target->number);
    if (color==1) printf("%03d:38 blue %s %d used a bomb and killed red %s %d\n",hour,warrior_name[job],number,warrior_name[target->job],target->number);
}
void warrior::use_arrow(warrior* target){
    target->life-=my_arrow->attack;
    my_arrow->life--;
    if (my_arrow->life==0) my_arrow=nullptr;
}

void warrior::report(){    
    if (color==0) printf("%03d:55 red %s %d has ",hour,warrior_name[job],number);
    else printf("%03d:55 blue %s %d has ",hour,warrior_name[job],number);
    bool hasweapon=0;
    if (my_arrow!=nullptr){
        if (hasweapon) printf(",");
        printf("arrow(%d)",my_arrow->life);
        hasweapon=1;
    }
    if (my_bomb!=nullptr){
        if (hasweapon) printf(",");
        printf("bomb");
        hasweapon=1;
    }
    if (my_sword!=nullptr){
        if (my_sword->life!=0){
            if (hasweapon) printf(",");
            printf("sword(%d)",my_sword->attack);
            hasweapon=1;
        }
        else my_sword=nullptr;
    }
    if (hasweapon) printf("\n");
    else printf("no weapon\n");
}

class dragon:public warrior{
public:
    double morale;//武士士气
    dragon(int new_color,int new_number,int new_job,int base_life):warrior(new_color,new_number,new_job){
        morale=double(base_life)/double(warrior_life[0]);
        life=warrior_life[0];
        attack=warrior_attack[0];
        printf("Its morale is %.2f\n",morale);
        get_weapon(new_number%3);  
    };
    void dragon_morale_change(bool ifwin){
        if (ifwin) morale+=0.2;
        else morale-=0.2;
    }
    void yell(int city_number){
        if (morale>0.8){
            if (color==1) printf("%03d:40 blue dragon %d yelled in city %d\n",hour,number,city_number);
            if (color==0) printf("%03d:40 red dragon %d yelled in city %d\n",hour,number,city_number);
        }
    }
};
class ninja:public warrior{
public:
    ninja(int new_color,int new_number,int new_job,int base_life):warrior(new_color,new_number,new_job){
        life=warrior_life[1];
        attack=warrior_attack[1];
        get_weapon(new_number%3);
        get_weapon((new_number+1)%3);
    } 
};
class iceman:public warrior{
public:
    bool step;
    iceman(int new_color,int new_number,int new_job,int base_life):warrior(new_color,new_number,new_job){
        life=warrior_life[2];
        attack=warrior_attack[2];
        get_weapon(new_number%3);
        step=0;
    }
    void life_decline(){
        if (step){
            life=max(life-9,1);
            attack+=20;
        }
        step=!step;
    } 
};
class lion:public warrior{
public:
    int loyalty;
    int life_before_battle;
    lion(int new_color,int new_number,int new_job,int base_life):warrior(new_color,new_number,new_job){
        life=warrior_life[3];
        life_before_battle=life;
        attack=warrior_attack[3];
        //get_weapon(new_number%3);
        loyalty=base_life;
        printf("Its loyalty is %d\n",loyalty);
    } 
    bool run_success(){
        if (color==0&&place==city_number+1) return false;
        if (color==1&&place==0) return false;
        if (loyalty<=0) return true;
        return false;
    }
    void loyalty_decline(bool ifwin){
        if (!ifwin) loyalty-=lion_loyalty_decline;
    }
    void record_life(){
        life_before_battle=max(life,0);
    }
    void lion_givelife(warrior* target){
        if (life<=0&&target->life>0) target->life+=life_before_battle;
    }
};
class wolf:public warrior{
public:
    wolf(int new_color,int new_number,int new_job,int base_life):warrior(new_color,new_number,new_job){
        life=warrior_life[4];
        attack=warrior_attack[4];
    }
    void steal(warrior* target){
        if (my_sword==nullptr) my_sword=target->my_sword;
        if (my_bomb==nullptr) my_bomb=target->my_bomb;
        if (my_arrow==nullptr) my_arrow=target->my_arrow;
    }
};

//城市类
class city{
public:
    int number;
    int life_total;//生命元
    int flag;
    int winner;
    vector<int> flag_record;//记录插旗进度,0红1蓝-1平局
    warrior* red_warrior;
    warrior* blue_warrior;
    warrior* new_red;
    warrior* new_blue;
    warrior* enemy_in_base;
    city(int new_number){
        number=new_number;
        red_warrior=nullptr;
        blue_warrior=nullptr;
        new_red=nullptr;
        new_blue=nullptr;
        life_total=0;
        flag=-1;
        flag_record.push_back(-1);
        enemy_in_base=nullptr;
    }

    //lion逃跑事件
    void run(){
        if (red_warrior!=nullptr&&red_warrior->run_success()){
            printf("%03d:05 red %s %d ran away\n",hour,warrior_name[red_warrior->job],red_warrior->number);
            red_warrior=nullptr;
        } 
        if (blue_warrior!=nullptr&&blue_warrior->run_success()){
            printf("%03d:05 blue %s %d ran away\n",hour,warrior_name[blue_warrior->job],blue_warrior->number);
            blue_warrior=nullptr;
        } 
    }
    //武士移动前清空new_red和new_blue
    void update_before_march(){
        new_red=nullptr;
        new_blue=nullptr;
    }
    //武士前进
    void march(){
        if (red_warrior!=nullptr){
            red_warrior->life_decline();
            //red_warrior->loyalty_decline();
            if (number<=city_number) map[number+1]->new_red=red_warrior;
            red_warrior=nullptr;
        }
        if (blue_warrior!=nullptr){
            blue_warrior->life_decline();
            //blue_warrior->loyalty_decline();
            if (number>0) map[number-1]->new_blue=blue_warrior;  
            blue_warrior=nullptr;
        }
        
    }
    //城市接受新武士
    void receive_new_warrior(){
        red_warrior=new_red;
        blue_warrior=new_blue;      
        if (red_warrior!=nullptr){
            red_warrior->place=number;
            if (number==city_number+1) {
                printf("%03d:10 red %s %d reached blue headquarter with %d elements and force %d\n",hour,warrior_name[red_warrior->job],red_warrior->number,red_warrior->life,red_warrior->attack);               
                blue_people++;
                enemy_in_base=red_warrior;
                red_warrior=nullptr;
                if (blue_people==2){
                    war_end=1;
                    printf("%03d:10 blue headquarter was taken\n",hour);
                }
            }
            else printf("%03d:10 red %s %d marched to city %d with %d elements and force %d\n",hour,warrior_name[red_warrior->job],red_warrior->number,number,red_warrior->life,red_warrior->attack);
        }
        if (blue_warrior!=nullptr){
            blue_warrior->place=number;
            if (number==0) {
                printf("%03d:10 blue %s %d reached red headquarter with %d elements and force %d\n",hour,warrior_name[blue_warrior->job],blue_warrior->number,blue_warrior->life,blue_warrior->attack);
                red_people++;
                enemy_in_base=blue_warrior;
                blue_warrior=nullptr;
                if (red_people==2){
                    printf("%03d:10 red headquarter was taken\n",hour);
                    war_end=1;
                }
            }
            else printf("%03d:10 blue %s %d marched to city %d with %d elements and force %d\n",hour,warrior_name[blue_warrior->job],blue_warrior->number,number,blue_warrior->life,blue_warrior->attack);
        }
    }
    //每个城市产出10生命
    void produce_life(){
        life_total+=10;
    }
    //武士取走生命元
    void get_life_before_battle(){
        if (red_warrior==nullptr&&blue_warrior!=nullptr){
            map[city_number+1]->life_total+=life_total;
            printf("%03d:30 blue %s %d earned %d elements for his headquarter\n",hour,warrior_name[blue_warrior->job],blue_warrior->number,life_total);
            life_total=0;
        }
        if (red_warrior!=nullptr&&blue_warrior==nullptr){
            map[0]->life_total+=life_total;
            printf("%03d:30 red %s %d earned %d elements for his headquarter\n",hour,warrior_name[red_warrior->job],red_warrior->number,life_total);
            life_total=0;
        }
    }
    //放箭
    void shoot(){
        if (red_warrior!=nullptr&&number!=city_number&&number!=city_number+1&&map[number+1]->blue_warrior!=nullptr&&red_warrior->my_arrow!=nullptr){
            red_warrior->use_arrow(map[number+1]->blue_warrior);
            if (map[number+1]->blue_warrior->life>0) printf("%03d:35 red %s %d shot\n",hour,warrior_name[red_warrior->job],red_warrior->number);
            else printf("%03d:35 red %s %d shot and killed blue %s %d\n",hour,warrior_name[red_warrior->job],red_warrior->number,warrior_name[map[number+1]->blue_warrior->job],map[number+1]->blue_warrior->number);
        }
        if (blue_warrior!=nullptr&&number!=1&&number!=0&&map[number-1]->red_warrior!=nullptr&&blue_warrior->my_arrow!=nullptr){
            blue_warrior->use_arrow(map[number-1]->red_warrior);
            if (map[number-1]->red_warrior->life>0) printf("%03d:35 blue %s %d shot\n",hour,warrior_name[blue_warrior->job],blue_warrior->number);
            else printf("%03d:35 blue %s %d shot and killed red %s %d\n",hour,warrior_name[blue_warrior->job],blue_warrior->number,warrior_name[map[number-1]->red_warrior->job],map[number-1]->red_warrior->number);
        }
    }
    bool ifusebomb=0;
    void bomb(){
        ifusebomb=0;
        if (red_warrior==nullptr||blue_warrior==nullptr) return; 
        if (red_warrior->life<=0||blue_warrior->life<=0) return;
        warrior red(*red_warrior);
        warrior blue(*blue_warrior); 
        int first_color=-1;
        if (flag==0){
            first_color=0;
        }
        if (flag==1){
            first_color=1;
        }
        if (flag==-1){
            if (number%2==1) {
                first_color=0;
            }
            if (number%2==0) {
                first_color=1;
            }
        }
        if (first_color==0){
            red.hit(&blue);
            if (blue.life>0) red.hit_back(&blue);
        }
        if (first_color==1){
            blue.hit(&red);
            if (red.life>0) blue.hit_back(&red);
        }
        if (red.life<=0&&red_warrior->my_bomb!=nullptr) {
            red_warrior->use_bomb(blue_warrior);
            ifusebomb=1;
        }
        if (blue.life<=0&&blue_warrior->my_bomb!=nullptr) {
            blue_warrior->use_bomb(red_warrior);
            ifusebomb=1;
        }
    }
    //战斗事件
    void battle(bool ifbattle){
        if (ifusebomb) {
            red_warrior=nullptr;
            blue_warrior=nullptr;
            return;
        }
        winner=-1;
        if (red_warrior==nullptr||blue_warrior==nullptr) return; 
        warrior* attacker;
        warrior* defender;    
        int attacker_color=-1;  
        if (flag==0){
            attacker=red_warrior;
            defender=blue_warrior;
            attacker_color=0;
        }
        if (flag==1){
            attacker=blue_warrior;
            defender=red_warrior;
            attacker_color=1;
        }
        if (flag==-1){
            if (number%2==1) {
                attacker=red_warrior;
                defender=blue_warrior;
                attacker_color=0;
            }
            if (number%2==0) {
                attacker=blue_warrior;
                defender=red_warrior;
                attacker_color=1;
            }
        }
        //记录战斗前生命
        red_warrior->record_life();
        blue_warrior->record_life();
        int attacker_life_before=attacker->life;
        int defender_life_before=defender->life;
        bool ifhit=0,ifhitback=0;
        if (defender->life>0&&attacker->life>0){
            attacker->hit(defender);
            ifhit=1;
            if (defender->life>0) {
                attacker->hit_back(defender);
                ifhitback=1;
            }
        }
        //bool attacker_use_bomb=0;
        //if (attacker_life_before>0&&attacker->life<=0&&attacker->my_bomb!=nullptr){
        //    attacker->use_bomb(defender);
        //    attacker_use_bomb=1;
        //}
        //if (attacker_use_bomb==0&&defender_life_before>0&&defender->life<=0&&defender->my_bomb!=nullptr)
        //    defender->use_bomb(attacker);   
        //if ((red_warrior->life<=0)&&(blue_warrior->life<=0)){
        //    red_warrior=nullptr;
        //    blue_warrior=nullptr;
        //    return;
        //}//用炸弹同归于尽

        if (!ifbattle) {
            //cout<<"*"<<endl;
            return;
        }
        if (ifhit) {
            if (attacker->color==0) printf("%03d:40 red %s %d attacked blue %s %d in city %d with %d elements and force %d\n",hour,warrior_name[red_warrior->job],red_warrior->number,warrior_name[blue_warrior->job],blue_warrior->number,number,attacker_life_before,red_warrior->attack);
            if (attacker->color==1) printf("%03d:40 blue %s %d attacked red %s %d in city %d with %d elements and force %d\n",hour,warrior_name[blue_warrior->job],blue_warrior->number,warrior_name[red_warrior->job],red_warrior->number,number,attacker_life_before,blue_warrior->attack);
        
        }
        if (ifhitback&&defender->job!=1){
            if (defender->color==0) printf("%03d:40 red %s %d fought back against blue %s %d in city %d\n",hour,warrior_name[red_warrior->job],red_warrior->number,warrior_name[blue_warrior->job],blue_warrior->number,number);
            if (defender->color==1) printf("%03d:40 blue %s %d fought back against red %s %d in city %d\n",hour,warrior_name[blue_warrior->job],blue_warrior->number,warrior_name[red_warrior->job],red_warrior->number,number);
        }
        if (blue_warrior->life<=0&&red_warrior->life>0){
            if (ifhit) printf("%03d:40 blue %s %d was killed in city %d\n",hour,warrior_name[blue_warrior->job],blue_warrior->number,number);
            red_warrior->steal(blue_warrior);
            blue_warrior->lion_givelife(red_warrior);
            blue_warrior=nullptr;
            red_warrior->dragon_morale_change(1);
            if (attacker_color==0) red_warrior->yell(number);
            flag_record.push_back(0);
            winner=0;
            victory_reward(0);
            get_life_after_battle();
            raise_flag();
            return;
        }
        if (red_warrior->life<=0&&blue_warrior->life>0){
            if (ifhit) printf("%03d:40 red %s %d was killed in city %d\n",hour,warrior_name[red_warrior->job],red_warrior->number,number);
            blue_warrior->steal(red_warrior);
            red_warrior->lion_givelife(blue_warrior);
            red_warrior=nullptr;
            blue_warrior->dragon_morale_change(1);
            if (attacker_color==1) blue_warrior->yell(number);
            flag_record.push_back(1);
            winner=1;
            victory_reward(1);
            get_life_after_battle();
            raise_flag();
            return;
        }
        if ((red_warrior->life>0)&&(blue_warrior->life>0)){
            //printf("%03d:40 both red %s %d and blue %s %d were alive in city %d\n",hour,warrior_name[red_warrior->job],red_warrior->number,warrior_name[blue_warrior->job],blue_warrior->number,number);
            red_warrior->dragon_morale_change(0);
            blue_warrior->dragon_morale_change(0);
            red_warrior->loyalty_decline(0);
            blue_warrior->loyalty_decline(0);
            if (attacker_color==0) red_warrior->yell(number);
            if (attacker_color==1) blue_warrior->yell(number);
            flag_record.push_back(-1);
            return;
        }
        //return;
    }
    virtual void produce(){};
    virtual void report_life(){};
    //报告武士情况事件
    void report_warrior(bool color){
        if (red_warrior!=nullptr&&red_warrior->life<=0) red_warrior=nullptr;
        if (blue_warrior!=nullptr&&blue_warrior->life<=0) blue_warrior=nullptr;
        if (color==0&&number==city_number+1&&enemy_in_base!=nullptr) enemy_in_base->report();
        if (color==1&&number==0&&enemy_in_base!=nullptr) enemy_in_base->report();
        if (color==0&&red_warrior!=nullptr) red_warrior->report();
        if (color==1&&blue_warrior!=nullptr) blue_warrior->report();
    }
    //奖励胜利武士生命元
    void victory_reward(bool color){
        if (color==0&&winner==0){
            if (map[0]->life_total>=8){
                map[0]->life_total-=8;
                red_warrior->life+=8;
            }
        }
        if (color==1&&winner==1){
            if (map[city_number+1]->life_total>=8){
                map[city_number+1]->life_total-=8;
                blue_warrior->life+=8;
            }
        }
    }
    void get_life_after_battle(){
        if (winner==0){
            map[0]->life_total+=life_total;
            printf("%03d:40 red %s %d earned %d elements for his headquarter\n",hour,warrior_name[red_warrior->job],red_warrior->number,life_total);
            life_total=0;
        }
        if (winner==1){
            map[city_number+1]->life_total+=life_total;
            printf("%03d:40 blue %s %d earned %d elements for his headquarter\n",hour,warrior_name[blue_warrior->job],blue_warrior->number,life_total);
            life_total=0;
        }
    }
    void raise_flag(){
        if (flag_record.back()==flag_record[flag_record.size()-2]){
            if (flag_record.back()==0){
                if (flag==0) return;
                flag=0;
                printf("%03d:40 red flag raised in city %d\n",hour,number);
            }
            if (flag_record.back()==1){
                if (flag==1) return;
                flag=1;
                printf("%03d:40 blue flag raised in city %d\n",hour,number);
            }
        }
    }
};
class base:public city{
public:
    int color;//基地颜色,0为红，1为蓝
    int warrior_current[5];//当前武士数量
    int next_warrior_sequence;//下一个要制造的武士顺序
    int next_warrior;//下一个要制造的武士编号
    int warrior_number;
    base(int new_life_total,int new_color):city(new_color?city_number+1:0){
        life_total=new_life_total;       
        memset(warrior_current,0,sizeof(warrior_current));
        next_warrior_sequence=0;
        color=new_color;
        warrior_number=0;
    }
    //魔兽4造兵
    void produce(){       
        if (color==0) next_warrior=red_cycle[next_warrior_sequence];
        if (color==1) next_warrior=blue_cycle[next_warrior_sequence];
        if (life_total>=warrior_life[next_warrior]){
            warrior_number++;
            warrior_current[next_warrior]++;
            life_total-=warrior_life[next_warrior];
            next_warrior_sequence=(next_warrior_sequence+1)%5;
            if (color==0) {
                switch(next_warrior){
                    case 0:red_warrior=new dragon(0,warrior_number,0,life_total);break;
                    case 1:red_warrior=new ninja(0,warrior_number,1,life_total);break;
                    case 2:red_warrior=new iceman(0,warrior_number,2,life_total);break;
                    case 3:red_warrior=new lion(0,warrior_number,3,life_total);break;
                    case 4:red_warrior=new wolf(0,warrior_number,4,life_total);break;
                }
            }
            if (color==1) {
                switch(next_warrior){
                    case 0:blue_warrior=new dragon(1,warrior_number,0,life_total);break;
                    case 1:blue_warrior=new ninja(1,warrior_number,1,life_total);break;
                    case 2:blue_warrior=new iceman(1,warrior_number,2,life_total);break;
                    case 3:blue_warrior=new lion(1,warrior_number,3,life_total);break;
                    case 4:blue_warrior=new wolf(1,warrior_number,4,life_total);break;
                }   
            }                       
        }
    }
    //基地报告生命元
    void report_life(){
        if (color==0) printf("%03d:50 %d elements in red headquarter\n",hour,life_total);
        else printf("%03d:50 %d elements in blue headquarter\n",hour,life_total);
    }
};

int main(){
    //freopen("Warcraft.in","r",stdin);
    //freopen("Warcraft.out","w",stdout);
    int test_num;
    
    scanf("%d",&test_num);
    for (int j=1;j<=test_num;j++){  
        int life;
        int time_end;
        war_end=0;
        blue_people=0;
        red_people=0;
        scanf("%d %d %d %d %d",&life,&city_number,&arrow_attack,&lion_loyalty_decline,&time_end);
        int hour_end=time_end/60;
        map[0]=new base(life,0);
        for (int i=1;i<=city_number;i++)
            map[i]=new city(i);
        map[city_number+1]=new base(life,1);
        for (int i=0;i<5;i++)
            scanf("%d",&warrior_life[i]);
        for (int i=0;i<5;i++)
            scanf("%d",&warrior_attack[i]);
        printf("Case %d:\n",j);
        for (hour=0;hour<=hour_end;hour++){
            int min_all=hour*60;
            map[0]->produce();
            map[city_number+1]->produce();

            min_all+=5;
            if (min_all>time_end) break; 
            for (int i=0;i<=city_number+1;i++)
                map[i]->run();

            min_all+=5;
            if (min_all>time_end) break; 
            for (int i=0;i<=city_number+1;i++)
                map[i]->update_before_march();
            for (int i=0;i<=city_number+1;i++)
                map[i]->march();
            for (int i=0;i<=city_number+1;i++)
                map[i]->receive_new_warrior();
            if (war_end==1) break;

            min_all+=10;
            if (min_all>time_end) break; 
            for (int i=1;i<=city_number;i++)
                map[i]->produce_life();
            
            min_all+=10;
            if (min_all>time_end) break; 
            for (int i=1;i<=city_number;i++)
                map[i]->get_life_before_battle();
            
            min_all+=5;
            if (min_all>time_end) break;
            for (int i=1;i<=city_number;i++)
                map[i]->shoot();
            
            //此处内涵使用bomb和battle两个事件
            min_all+=3;
            if (min_all>time_end) break;
            for (int i=0;i<=city_number+1;i++){
                map[i]->bomb();
            }

            min_all+=2;
            if (min_all>time_end) break;
            for (int i=0;i<=city_number+1;i++){
                map[i]->battle(1);
            }

            //if (min_all>time_end) break;
            //for (int i=1;i<=city_number;i++){
            //    map[i]->victory_reward(1);
            //}
            //for (int i=city_number;i>=1;i--){
            //    map[i]->victory_reward(0);
            //}
            //for (int i=1;i<=city_number;i++){
            //    map[i]->get_life_after_battle();
            //}
            //for (int i=1;i<=city_number;i++){
            //    map[i]->get_life_after_battle();
            //}
            //for (int i=1;i<=city_number;i++){
            //    map[i]->raise_flag();
            //}
            
            min_all+=10;
            if (min_all>time_end) break;
            for (int i=0;i<=city_number+1;i++)
                map[i]->report_life();
            min_all+=5;
            if (min_all>time_end) break;
            for (int i=0;i<=city_number+1;i++)
                map[i]->report_warrior(0);
            for (int i=0;i<=city_number+1;i++)
                map[i]->report_warrior(1);
        }
    }
    return 0;
}
