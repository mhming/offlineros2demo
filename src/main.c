/*************************************************************************************
@company: WHEELTEC (Dongguan) Co., Ltd
@product: 4/6mic
@filename: main.c
@brief:
@version:       date:       author:            comments:
@v1.0           22-4-16      hj,choi                main
*************************************************************************************/

#include "com_test.h"
#include "record.h"
#include "asr_offline_record_sample.h"

/********************************[1]　运行效果调试参数****************************************/
int confidence = 40; //置信度阈值，可根据麦克风使用环境进行设置，若容易检测出错，则调大该值。
int time_per_order = 5;//一次录音时长
int awake_count = 5;//一次唤醒，可允许对话的次数

extern char *ASR_RES_PATH; //离线语法识别资源路径，重要，与麦克风及appid绑定
extern char *GRM_BUILD_PATH;   //构建离线语法识别网络生成数据保存路径
extern char *GRM_FILE;					//构建离线识别语法网络所用的语法文件，用户自修改文件
extern char *LEX_NAME;
extern char *APPID; //APPID

extern int angle_int;
extern int if_awake;

int once_awake;
init_success = 0;
int wake_switch = 1;

void *com_wakeup(void* args)
{
    int fd=1, read_num = 0;
    unsigned char buffer[1];
    memset(buffer, 0, 1);
    char* uartname="/dev/padroid_mic";
    if((fd=open_port(uartname))<0)
    {
         printf("open %s is failed\n",uartname);
         return 0;
    }
    else{
            set_opt(fd, 115200, 8, 'N', 1);
            //printf("set_opt fd=%d\n",fd);

            while(1){
                    usleep(1000);
                    memset(buffer, 0, 1);
                    read_num = read(fd, buffer, 1);
                    //printf("read_num=%d\n",read_num);
                    if(read_num>0){
						//printf("%x\n", buffer[0]);
						deal_with(buffer[0]);
                    }
                    //else{
                    //    printf("read error\n");
                    //}
                    if(if_awake){
						//printf("angle : [%d]\n",angle_int);
                        sleep(0.5);
						if_awake = 0;
                        wake_switch = 0;
					}    
            }
          fd=close(fd);
    }
    return 0;
}

void *time_count(void* args)
{
    while(1){
            if (!init_success)
            {
                sleep(time_per_order);
                time_per = 1;     
            }
            
    }
    return 0;
}

/********************获取识别到的内容里关键词、置信度以及关键词的id******************/
Effective_Result show_result(const char *string, const int input_confidence)
{
    Effective_Result current;
    int confidence_int = 0;
    int order_id = 0;

    if (strlen(string) > 250)
    {
        char asr_result[32];    //识别到的关键字的结果
        char asr_confidence[3]; //识别到的关键字的置信度
        char str_orderID[8];    //识别到的关键字的id

        char *p1 = strstr(string, "<rawtext>");
        char *p2 = strstr(string, "</rawtext>");
        int n1 = p1 - string + 1;
        int n2 = p2 - string + 1;

        char *p3 = strstr(string, "<confidence>");
        char *p4 = strstr(string, "</confidence>");
        int n3 = p3 - string + 1;
        int n4 = p4 - string + 1;
        for (int i = 0; i < 32; i++)
        {
            asr_result[i] = '\0';
        }

        strncpy(asr_confidence, string + n3 + strlen("<confidence>") - 1, n4 - n3 - strlen("<confidence>"));
        asr_confidence[n4 - n3 - strlen("<confidence>")] = '\0';
        confidence_int = atoi(asr_confidence);

        if (confidence_int >= input_confidence)
        {
            strncpy(asr_result, string + n1 + strlen("<rawtext>") - 1, n2 - n1 - strlen("<rawtext>"));
            asr_result[n2 - n1 - strlen("<rawtext>")] = '\0'; //加上字符串结束符。

            memset(str_orderID, 0, sizeof(str_orderID));
            char *str_todo = strstr(p3, "id=");
            char *str_todo_back = strstr(str_todo, ">");
            strncpy(str_orderID, str_todo + 4, str_todo_back - str_todo - 5);
            order_id = atoi(str_orderID);
        }
        else
        {
            strncpy(asr_result, "", 0);
            order_id = 0;
        }

        current.effective_confidence = confidence_int;
        strcpy(current.effective_word, asr_result);
        current.effective_id = order_id;
        return current;
    }
    else
    {
        current.effective_confidence = 0;
        strcpy(current.effective_word, " ");
        current.effective_id = 0;
        return current;
    }
}
 
int main()
{
    pthread_t pid1;
    pthread_t pid2;
 
    if(pthread_create(&pid1, NULL, com_wakeup, NULL))
    {
        return -1;
    }

    if(pthread_create(&pid2, NULL, time_count, NULL))
    {
        return -1;
    }
 
    while(1){

        if (!if_awake && wake_switch)
        {
            printf(">>>>>待唤醒，请用唤醒词进行唤醒！\n");
        }
        while(!if_awake && wake_switch)
        {
            sleep(1);
        }
        printf("\n>>>>>唤醒成功，已开启录音！\n");
        printf("angle : [%d]\n",angle_int);
        once_awake = awake_count;
        wake_switch = 1;
        
        while(once_awake)
        {
            time_per = 0;
            once_awake--;
            const char file1[] = DENOISE_SOUND_PATH;
            remove(file1);

            get_the_record_sound(file1);

            unsigned char file[] = DENOISE_SOUND_PATH;
            Recognise_Result result = deal_with_record(file, ASR_RES_PATH, GRM_BUILD_PATH, GRM_FILE, LEX_NAME);
            printf(">>>>>正在识别...\n");

            if (result.whether_recognised)
            {
                printf(">>>>>是否识别成功:　[ %s ]\n", "是");
                if (result.whole_content)
                {
                    /**为了在终端可视化方便,可将本行注释，若需查看完整反馈，可解除屏蔽**/
                    //printf(">>>>>全部返回结果:　[ %s ]\n", result.whole_content);
                    Effective_Result effective_ans = show_result(result.whole_content, confidence); //
                    printf(">>>>>关键字的置信度: [ %d ]\n", effective_ans.effective_confidence);
                    if (effective_ans.effective_confidence >= confidence) //如果大于置信度阈值则进行显示或者其他控制操作
                    {
                        printf(">>>>>关键字识别结果: [ %s ]\n", effective_ans.effective_word);
                    }
                    else
                    {
                        printf(">>>>>关键字置信度较低，文本不予显示\n");
                    }
                }
                else
                {
                    printf(">>>>>未能检测到有效声音,请重试\n");
                }
            }
            else
            {
                printf(">>>>>是否识别成功:　[ %s ]\n", "否");
                printf(">>>>>错误原因: [ %s ]\n", result.fail_reason);
            }
            
            if (!wake_switch)
            {
                break;
            }
    }
    sleep(1);
 }
    return 0;
}