/************************************************
 神舟号 STM32F407开发板 
 安徽中科康芯科技有限公司  
************************************************/
#include "main.h"
extern u8 USART3_RX_BUF[];
extern u16 USART3_RX_STA;

// 命令ID定义
#define CMD_BLOOD_PRESSURE    3    // 量血压
#define CMD_HELLO             2    // 你好
#define CMD_GOODBYE           999  // 再见
#define CMD_WHO_ARE_YOU       66   // 你叫什么名字
#define CMD_HEART_RATE        6    // 测心率
#define CMD_TEMPERATURE       20   // 测体温

// 字库结构体定义
typedef struct {
    int id;
    const char* text;
} CmdLibrary;

// 字库定义
const CmdLibrary cmd_library[] = {
    {3, "我要量血压"},
    {2, "你好"},
    {999, "再见"},
    {66, "你叫什么名字"},
    {6, "我要测心率"},
    {20, "我要测体温"}
};

const int cmd_library_size = sizeof(cmd_library) / sizeof(cmd_library[0]);

// 根据命令文本查找命令ID
int find_cmd_id(const char* cmd_text) {
    int i;
    for(i = 0; i < cmd_library_size; i++) {
        if(strstr(cmd_text, cmd_library[i].text) != NULL) {
            return cmd_library[i].id;
        }
    }
    return -1; // 未找到
}

// 根据命令ID获取响应文本
char* get_response_by_cmd_id(int cmd_id) {
    switch(cmd_id) {
        case CMD_HELLO:
            return "你好！有什么可以帮助你的吗？\r\n";
        case CMD_GOODBYE:
            return "再见！祝你身体健康！\r\n";
        case CMD_WHO_ARE_YOU:
            return "我是神舟号智能健康检测系统。\r\n";
        case CMD_HEART_RATE:
            return "正在为你测量心率...\r\n";
        case CMD_BLOOD_PRESSURE:
            return "正在为你测量血压...\r\n";
        case CMD_TEMPERATURE:
            return "正在为你测量体温...\r\n";
        default:
            return "未知命令，请重试。\r\n";
    }
}

void Program_Init(void)
{
	/*抢占优先级2位，响应优先级1位*/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	//Exti4_Init();
	Usart1_Init(9600); //在串口助手上用printf打印数据需要调用此函数	
	AS6221_IIC_Init(); //温度传感器初始化
	MKS141_Usart2_Init(38400); //心率血氧血压综合检测模块，只能是38400
	Usart3_Init(115200); //通信串口---与arm平台通信
	printf("神舟号智能健康系统初始化完成。\r\n");
	printf("系统就绪，等待命令...\r\n");
}

//程序的人口
// 主函数
// 主函数示例
int main(void)
{
    MKS141_Date mk;
    float temperature = 0.0;
    int cmd_id = -1;
    int measurement_done = 0;
    char* response;
    
    Program_Init();
    
    while(1)
    {
        // 检测是否接收到命令
        if(USART3_RX_STA)
        {
            Usart_SendString(USART1, (char *)USART3_RX_BUF);
            
            // 首先进行命令识别
            cmd_id = find_cmd_id((const char*)USART3_RX_BUF);
            
            if(cmd_id != -1) {
                // 发送响应消息
                response = get_response_by_cmd_id(cmd_id);
                Usart_SendString(USART3, response);
                
                switch(cmd_id) {
                    case CMD_BLOOD_PRESSURE:
                        // 测量血压
                        measurement_done = 1;
                        memset(USART3_RX_BUF, 0, sizeof(USART3_RX_BUF));
                        USART3_RX_STA = 0;
                        
                        // 持续测量血压
                        while(measurement_done) {
                            mk = Get_MKS141_Date(); //获取血压数值
                            if(mk.SBP && mk.DBP) {
                                // 发送血压数据到ARM平台
                                Usart_SendString(USART3, "收缩压|");
                                printf("%d", mk.SBP);
                                Usart_SendString(USART3, "|舒张压|");
                                printf("%d", mk.DBP);
                                Usart_SendString(USART3, "\r\n");
                                measurement_done = 0;
                                break;
                            }
                            delay_s(1);
                        }
                        break;
                        
                    case CMD_HEART_RATE:
                        // 测量心率
                        measurement_done = 1;
                        memset(USART3_RX_BUF, 0, sizeof(USART3_RX_BUF));
                        USART3_RX_STA = 0;
                        
                        // 持续测量心率
                        while(measurement_done) {
                            mk = Get_MKS141_Date(); //获取心率数值
                            if(mk.HR) {
                                // 发送心率数据到ARM平台
                                Usart_SendString(USART3, "心率|");
                                printf("%d", mk.HR);
                                Usart_SendString(USART3, "|次/分\r\n");
                                measurement_done = 0;
                                break;
                            }
                            delay_s(1);
                        }
                        break;
                        
                    case CMD_TEMPERATURE:
                        // 测量体温
                        temperature = AS6221_GetTemperature();
                        Usart_SendString(USART3, "体温|");
                        printf("%.2f", temperature);
                        Usart_SendString(USART3, "|℃\r\n");
                        break;
                        
                    case CMD_GOODBYE:
                        // 再见命令
                        break;
                        
                    case CMD_HELLO:
                        // 你好命令，响应已发送
                        break;
                        
                    case CMD_WHO_ARE_YOU:
                        // 询问身份命令，响应已发送
                        break;
                        
                    default:
                        Usart_SendString(USART3, "未知命令，请重试。\r\n");
                        break;
                }
            } else {
                // 未识别命令
                Usart_SendString(USART3, "命令未识别，请说出: 你好、我要测心率、我要量血压、我要测体温、你叫什么名字、再见\r\n");
            }
            
            // 清除接收缓冲区
            memset(USART3_RX_BUF, 0, sizeof(USART3_RX_BUF));
            USART3_RX_STA = 0;
        }
        
        delay_s(1);
    }
}