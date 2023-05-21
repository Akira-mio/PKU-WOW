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
    vector<weapon*> weapon_bag;
    int place;
    //int loyalty=-1;//武士忠诚度
    warrior(int new_color,int new_number,int new_job);
    void get_weapon(int weapon_number);//被造出时获得武器
    void sort_weapon_beforesteal();//被偷，被缴获前排武器顺序
    void sort_weapon_beforebattle();//打架前排武器顺序
    virtual void loyalty_decline(){};//lion忠诚减损
    virtual void life_decline(){};//iceman生命减损
    virtual void steal(warrior* target){};//wolf偷武器
    virtual void yell(int city_number){};//dragon欢呼
    virtual bool run_success(){return false;}//lion是否成功逃跑
    void use_weapon(warrior* defender);//武士拿武器攻击
    bool stop();//武士这一轮攻击完毕（武器用完或不停用sword刮痧）
    void victory_get_weapon(warrior* target);//胜利时缴获武器
    void report();//报告武器和生命
};

//武器类
class weapon{
public:
    int categlory;
    int attack;
    int life;
    weapon(int new_categlory){
        categlory=new_categlory;
    }
    virtual void hit(warrior* user,warrior* defender)=0;//用武器打一下

};
class sword:public weapon{
public:
    sword(int new_categlory):weapon(new_categlory){
        life=MAXINT;
    }
    void hit(warrior* user,warrior* defender){
        attack=user->attack/5;
        defender->life-=attack;

    }
};
class bomb:public weapon{
public:
    bomb(int new_categlory):weapon(new_categlory){
        life=1;
    }
    void hit(warrior* user,warrior* defender){
        attack=user->attack*2/5;
        defender->life-=attack;
        if (user->job!=1) user->life-=attack/2;
        life--;
    }
};
class arrow:public weapon{
public:
    arrow(int new_categlory):weapon(new_categlory){
        life=2;
    }
    void hit(warrior* user,warrior* defender){
        attack=user->attack*3/10;
        defender->life-=attack;
        life--;
    }
};
bool weapon_cmp_beforesteal(weapon* w1,weapon*w2){
    if (w1->categlory<w2->categlory) return true;
    if (w1->categlory>w2->categlory) return false;
    if (w1->categlory==2){
        if (w1->life>w2->life) return true;
        if (w1->life<w2->life) return false;
    }
    return true;
}
bool weapon_cmp_beforebattle(weapon* w1,weapon*w2){
    if (w1->categlory<w2->categlory) return true;
    if (w1->categlory>w2->categlory) return false;
    if (w1->categlory==2){
        if (w1->life<w2->life) return true;
        if (w1->life>w2->life) return false;
    }
    return true;
}



warrior::warrior(int new_color,int new_number,int new_job){
    color=new_color;
    number=new_number;
    job=new_job;
    if (color==0) place=0;
        else place=city_number+1;
    if (color==0) printf("%03d:00 red %s %d born\n",hour,warrior_name[job],number);
    else printf("%03d:00 blue %s %d born\n",hour,warrior_name[job],number);
}
void warrior::get_weapon(int weapon_number){
    switch (weapon_number){
        case 0:weapon_bag.push_back(new sword(0));break;
        case 1:weapon_bag.push_back(new bomb(1));break;
        case 2:weapon_bag.push_back(new arrow(2));break;
    }
}
void warrior::sort_weapon_beforesteal(){
    sort(weapon_bag.begin(),weapon_bag.end(),weapon_cmp_beforesteal);
}
void warrior::sort_weapon_beforebattle(){
    sort(weapon_bag.begin(),weapon_bag.end(),weapon_cmp_beforebattle);
}
void warrior::use_weapon(warrior* defender){    
    weapon* temp=weapon_bag[0];
    weapon_bag.erase(weapon_bag.begin());
    temp->hit(this,defender);
    if (temp->life!=0) weapon_bag.push_back(temp);
}
bool warrior::stop(){
    if (weapon_bag.size()==0) return true;
    if (attack/5>0) return false;
    for (int i=0;i<weapon_bag.size();i++)
        if (weapon_bag[i]->categlory!=0) return false;
    return true;
}
void warrior::victory_get_weapon(warrior* target){
    while (weapon_bag.size()<=10&&target->weapon_bag.size()>0){
        weapon_bag.push_back(target->weapon_bag[0]);
        target->weapon_bag.erase(target->weapon_bag.begin());
    }
}
void warrior::report(){
    int weapon_number[3]={0};
    for (int i=0;i<weapon_bag.size();i++){
        weapon_number[weapon_bag[i]->categlory]++;
    }
    if (color==0) printf("%03d:55 red %s %d has %d sword %d bomb %d arrow and %d elements\n",hour,warrior_name[job],number,weapon_number[0],weapon_number[1],weapon_number[2],life);
    else printf("%03d:55 blue %s %d has %d sword %d bomb %d arrow and %d elements\n",hour,warrior_name[job],number,weapon_number[0],weapon_number[1],weapon_number[2],life);
}

class dragon:public warrior{
public:
    double morale;//武士士气
    dragon(int new_color,int new_number,int new_job,int base_life):warrior(new_color,new_number,new_job){
        morale=double(base_life)/double(warrior_life[0]);
        life=warrior_life[0];
        attack=warrior_attack[0];
        get_weapon(new_number%3);  
    };
    void yell(int city_number){
        if (color==1) printf("%03d:40 blue dragon %d yelled in city %d\n",hour,number,city_number);
        if (color==0) printf("%03d:40 red dragon %d yelled in city %d\n",hour,number,city_number);
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
    iceman(int new_color,int new_number,int new_job,int base_life):warrior(new_color,new_number,new_job){
        life=warrior_life[2];
        attack=warrior_attack[2];
        get_weapon(new_number%3);
    }
    void life_decline(){
        life-=life/10;
    } 
};
class lion:public warrior{
public:
    int loyalty;
    lion(int new_color,int new_number,int new_job,int base_life):warrior(new_color,new_number,new_job){
        life=warrior_life[3];
        attack=warrior_attack[3];
        get_weapon(new_number%3);
        loyalty=base_life;
        printf("Its loyalty is %d\n",loyalty);
    } 
    bool run_success(){
        if (color==0&&place==city_number+1) return false;
        if (color==1&&place==0) return false;
        if (loyalty<=0) return true;
        return false;
    }
    void loyalty_decline(){
        loyalty-=lion_loyalty_decline;
    }
};
class wolf:public warrior{
public:
    wolf(int new_color,int new_number,int new_job,int base_life):warrior(new_color,new_number,new_job){
        life=warrior_life[4];
        attack=warrior_attack[4];
    }
    void steal(warrior* target){
        if (target->job==4) return;
        if (target->weapon_bag.size()>0){
            int steal_categlory=target->weapon_bag[0]->categlory;
            int steal_number=0;
            while (weapon_bag.size()<=10&&target->weapon_bag.size()>0&&target->weapon_bag[0]->categlory==steal_categlory){
                weapon_bag.push_back(target->weapon_bag[0]);
                target->weapon_bag.erase(target->weapon_bag.begin());
                steal_number++;
            }
            if (color==0) if (steal_number>0) printf("%03d:35 red %s %d took %d %s from blue %s %d in city %d\n",hour,warrior_name[job],number,steal_number,weapon_name[steal_categlory],warrior_name[target->job],target->number,place);
            if (color==1) if (steal_number>0) printf("%03d:35 blue %s %d took %d %s from red %s %d in city %d\n",hour,warrior_name[job],number,steal_number,weapon_name[steal_categlory],warrior_name[target->job],target->number,place);
        }
    } 
};

//城市类
class city{
public:
    int number;
    warrior* red_warrior;
    warrior* blue_warrior;
    warrior* new_red;
    warrior* new_blue;
    city(int new_number){
        number=new_number;
        red_warrior=nullptr;
        blue_warrior=nullptr;
        new_red=nullptr;
        new_blue=nullptr;
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
            red_warrior->loyalty_decline();
            if (number<=city_number) map[number+1]->new_red=red_warrior;
            red_warrior=nullptr;
        }
        if (blue_warrior!=nullptr){
            blue_warrior->life_decline();
            blue_warrior->loyalty_decline();
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
                printf("%03d:10 blue headquarter was taken\n",hour);
                war_end=1;
            }
            else printf("%03d:10 red %s %d marched to city %d with %d elements and force %d\n",hour,warrior_name[red_warrior->job],red_warrior->number,number,red_warrior->life,red_warrior->attack);
        }
        if (blue_warrior!=nullptr){
            blue_warrior->place=number;
            if (number==0) {
                printf("%03d:10 blue %s %d reached red headquarter with %d elements and force %d\n",hour,warrior_name[blue_warrior->job],blue_warrior->number,blue_warrior->life,blue_warrior->attack);
                printf("%03d:10 red headquarter was taken\n",hour);
                war_end=1;
            }
            else printf("%03d:10 blue %s %d marched to city %d with %d elements and force %d\n",hour,warrior_name[blue_warrior->job],blue_warrior->number,number,blue_warrior->life,blue_warrior->attack);
        }
    }
    //wolf偷武器事件
    void steal_weapon(){
        if (red_warrior==nullptr||blue_warrior==nullptr) return;
        red_warrior->sort_weapon_beforesteal();
        blue_warrior->sort_weapon_beforesteal();
        red_warrior->steal(blue_warrior);
        blue_warrior->steal(red_warrior);
    }
    //战斗事件
    void battle(){
        if (red_warrior==nullptr||blue_warrior==nullptr) return;       
        red_warrior->sort_weapon_beforebattle();
        blue_warrior->sort_weapon_beforebattle();
        bool red_end=!red_warrior->weapon_bag.size(),blue_end=!blue_warrior->weapon_bag.size();
        int winner=-1;
        if (number%2!=0){
            while ((!red_end)||(!blue_end)){
                if (!red_end) red_warrior->use_weapon(blue_warrior);
                if ((red_warrior->life<=0)||(blue_warrior->life<=0)) break;
                if (red_warrior->stop()) red_end=true;
                if (!blue_end) blue_warrior->use_weapon(red_warrior);
                if ((red_warrior->life<=0)||(blue_warrior->life<=0)) break;
                if (blue_warrior->stop()) blue_end=true;
            }
        }
        else{
            while ((!red_end)||(!blue_end)){
                if (!blue_end) blue_warrior->use_weapon(red_warrior);
                if ((red_warrior->life<=0)||(blue_warrior->life<=0)) break;
                if (blue_warrior->stop()) blue_end=true;
                if (!red_end) red_warrior->use_weapon(blue_warrior);
                if ((red_warrior->life<=0)||(blue_warrior->life<=0)) break;
                if (red_warrior->stop()) red_end=true;
            }
        }
        if ((red_warrior->life<=0)&&(blue_warrior->life<=0)){
            printf("%03d:40 both red %s %d and blue %s %d died in city %d\n",hour,warrior_name[red_warrior->job],red_warrior->number,warrior_name[blue_warrior->job],blue_warrior->number,number);
            red_warrior=nullptr;
            blue_warrior=nullptr;
            return;
        }
        if (blue_warrior->life<=0){
            printf("%03d:40 red %s %d killed blue %s %d in city %d remaining %d elements\n",hour,warrior_name[red_warrior->job],red_warrior->number,warrior_name[blue_warrior->job],blue_warrior->number,number,red_warrior->life);
            blue_warrior->sort_weapon_beforesteal();
            red_warrior->victory_get_weapon(blue_warrior);
            blue_warrior=nullptr;
            red_warrior->yell(number);
            return;
        }
        if (red_warrior->life<=0){
            printf("%03d:40 blue %s %d killed red %s %d in city %d remaining %d elements\n",hour,warrior_name[blue_warrior->job],blue_warrior->number,warrior_name[red_warrior->job],red_warrior->number,number,blue_warrior->life);
            red_warrior->sort_weapon_beforesteal();
            blue_warrior->victory_get_weapon(red_warrior);
            red_warrior=nullptr;
            blue_warrior->yell(number);
            return;
        }
        if ((red_warrior->life>0)&&(blue_warrior->life>0)){
            printf("%03d:40 both red %s %d and blue %s %d were alive in city %d\n",hour,warrior_name[red_warrior->job],red_warrior->number,warrior_name[blue_warrior->job],blue_warrior->number,number);
            red_warrior->yell(number);
            blue_warrior->yell(number);
            return;
        }
    }
    virtual void produce(){};
    virtual void report_life(){};
    //报告武士情况事件
    void report_warrior(){
        if (red_warrior!=nullptr) red_warrior->report();
        if (blue_warrior!=nullptr) blue_warrior->report();
    }
};
class base:public city{
public:
    int color;//基地颜色,0为红，1为蓝
    int life_total;//生命元
    int warrior_current[5];//当前武士数量
    int next_warrior_sequence;//下一个要制造的武士顺序
    int next_warrior;//下一个要制造的武士编号
    bool stop=0;//是否停止制造武士
    base(int new_life_total,int new_color):city(new_color?city_number+1:0){
        life_total=new_life_total;       
        memset(warrior_current,0,sizeof(warrior_current));
        next_warrior_sequence=0;
        color=new_color;
    }
    //魔兽2的造兵，目前没用
    void produce2(){
        int warrior_number=hour+1;//武士编号（制造顺序）
        for (int i=0;i<5;i++){
            if (color==0) next_warrior=red_cycle[next_warrior_sequence];
            if (color==1) next_warrior=blue_cycle[next_warrior_sequence];
            if (life_total>=warrior_life[next_warrior]){
                warrior_current[next_warrior]++;
                life_total-=warrior_life[next_warrior];
                next_warrior_sequence=(next_warrior_sequence+1)%5;
                if (color==0) {
                    printf("%03d red %s %d born with strength %d,%d %s in red headquarter\n",hour,warrior_name[next_warrior],hour+1,warrior_life[next_warrior],warrior_current[next_warrior],warrior_name[next_warrior]);
                }
                if (color==1) {
                    printf("%03d blue %s %d born with strength %d,%d %s in blue headquarter\n",hour,warrior_name[next_warrior],hour+1,warrior_life[next_warrior],warrior_current[next_warrior],warrior_name[next_warrior]);
                } 
                switch(next_warrior){
                    case 0:printf("It has a %s,and it's morale is %.2f\n",weapon_name[warrior_number%3],double(life_total)/double(warrior_life[next_warrior]));break;
                    case 1:printf("It has a %s and a %s\n",weapon_name[warrior_number%3],weapon_name[(warrior_number+1)%3]);break;
                    case 2:printf("It has a %s\n",weapon_name[warrior_number%3]);break;
                    case 3:printf("It's loyalty is %d\n",life_total);break;
                }
                return;                 
            }
            next_warrior_sequence=(next_warrior_sequence+1)%5;
        }
        if (color==0) printf("%03d red headquarter stops making warriors\n",time);
        if (color==1) printf("%03d blue headquarter stops making warriors\n",time);
        next_warrior_sequence=-1;
    }
    //魔兽3造兵
    void produce(){
        if (stop) return;
        int warrior_number=hour+1;
        if (color==0) next_warrior=red_cycle[next_warrior_sequence];
        if (color==1) next_warrior=blue_cycle[next_warrior_sequence];
        if (life_total>=warrior_life[next_warrior]){
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
        else stop=1;
    }
    //基地报告生命元
    void report_life(){
        if (color==0) printf("%03d:50 %d elements in red headquarter\n",hour,life_total);
        else printf("%03d:50 %d elements in blue headquarter\n",hour,life_total);
    }
};

int main(){
    int test_num;
    
    scanf("%d",&test_num);
    for (int j=1;j<=test_num;j++){  
        int life;
        int time_end;
        war_end=0;
        scanf("%d %d %d %d",&life,&city_number,&lion_loyalty_decline,&time_end);
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
            min_all+=25;
            if (min_all>time_end) break;
            for (int i=0;i<=city_number+1;i++)
                map[i]->steal_weapon();
            min_all+=5;
            if (min_all>time_end) break;
            for (int i=0;i<=city_number+1;i++)
                map[i]->battle();
            min_all+=10;
            if (min_all>time_end) break;
            for (int i=0;i<=city_number+1;i++)
                map[i]->report_life();
            min_all+=5;
            if (min_all>time_end) break;
            for (int i=0;i<=city_number+1;i++)
                map[i]->report_warrior();
        }
    }
    return 0;
}
