using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using System.Text.RegularExpressions;
using System.IO.Ports;


namespace RS485_ModBus
{
    public partial class Form1 : Form
    {
        private StringBuilder recbuf = new StringBuilder();
        private StringBuilder funccode = new StringBuilder();
        private StringBuilder DAC = new StringBuilder();
        public uint startRegAddr;
        public uint RegNum;

        public static int b10=1;
        public static int b11=1;
        public static int b12=1;
        public static int b13=1;
        public static int b14=1;
        public static int b20=1;
        public static int b21=1;

        public Form1()
        {
            InitializeComponent();
            this.Load += new EventHandler(Form1_Load);
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            comboBox1.Items.AddRange(System.IO.Ports.SerialPort.GetPortNames());
            this.comboBox2.Items.Add("9600");
            this.comboBox3.Items.Add("8");
            this.comboBox4.Items.Add("None");
            this.comboBox5.Items.Add("1");
            this.comboBox1.SelectedIndex = 0;
            this.comboBox2.SelectedIndex = 0;
            this.comboBox3.SelectedIndex = 0;
            this.comboBox4.SelectedIndex = 0;
            this.comboBox5.SelectedIndex = 0;
            this.textBox_Receive.AutoSize = false;
            this.textBox_Send.AutoSize = false;
            this.textBox_Send.Height = 18;
            this.textBox_Receive.Height = 18;
            button8.Text = "打开串口";
            button8.BackColor = Color.ForestGreen;
            Modbus_struct.Send.SensorID = 0x01;
        }
        public modbus Modbus_struct;
        public struct modbus
        {

            public data Send;                                   // 传感器发送参数
            public data Rece;                                   // 传感器接收参数
            public string[,] SendStr;                           // 发送字符表
            public string[,] RecStr;                            // 接收字符表

            public struct data
            {
                public uint SensorID;                           // 传感器ID
                public uint FunCode;                            // 功能码
                public uint StartAddr;                          // 起始地址
                public uint RegNum;                             // 寄存器数量
                public uint ByteNum;                            // 字节数
                public uint[] Data;                             // 数据
                public uint ErrorNum;                           // 错误编号
                public uint CRC;                                // CRC校验
                public uint LRC;                                // LRC校验

                public string sSensorID;                        // 传感器ID
                public string sFunCode;                         // 功能码
                public string sStartAddr;                       // 起始地址
                public string sRegNum;                          // 寄存器数量
                public string sByteNum;                         // 字节数
                public string[] sData;                          // 数据
                public string sErrorNum;                        // 错误编号
                public string sCRC;                             // CRC校验
                public string sLRC;                             // LRC校验

                public byte[] RtuCommand;                       // 命令数组
                public uint RtuLen;                             // 命令数组长度
                public string sRtuCommand;                      // 命令字符串 将一条指令合并成字符串 字节数据间以空格分开
                public string[] sRtuCommandArray;               // 命令字符串数组 存储指令每字节数据字符

                public byte[] AsciiCommand;
                public uint AsciiLen;
                public string sAsciiCommand;
                public string[] sAsciiCommandArray;

                public bool AnalyzeFinsh;                       // 解析完成
                public uint[] MsgLen;                           // 信息长度
                public string[] MsgStr;                         // 信息字符串      
                public byte[] RecData;
                public uint CorrectRecLen;                      // 正确回复长度
                public uint RecLen;                             // 实际接收长度
            }
        }

        public struct inout
        {
            public bool IsTrue;                                 // 输入数据是否正确
            public byte[] InputData;                            // 输入数据
            public uint InputLen;                               // 输入长度
            public string[] InputStr;                           // 输入数据字符串

            public frameerror FrameError;                       // 帧错误码
            public string NormalStyle;                          // 正常模式为RTU模式
            public byte[] ConvData;                             // 转换字符串
            public uint ConvLen;                                // 转换数据长度
            public string ConvStr;                              // 转换字符串
            public string[] ConvStrArray;                       // 转换数据字符串
        }

        public enum frameerror
        {
            None = 0,                                           // 无错误
            SenorIDOverflow,                                    // 传感器地址溢出     正常范围是 0~247 
            FunCodeError,                                       // 功能码错误         仅支持 03/10 04 05 25 26/27 功能码
            StartAddrError,                                     // 起始ID错误         04功能码起始地址必须为偶数
            RegNumError,                                        // 寄存器数量错误     连读时
            ByteNumError,                                       // 字节数错误
            FrameHeaderError,                                   // 帧头错误
            FrameTailError,                                     // 帧尾错误
            FrameLenError,                                      // 帧长度错误
            CRCError,                                           // CRC校验错误
            LRCError,                                           // LRC校验错误
        }

        public enum strerror
        {
            None = 0,
            CharError,
            LenError,
            TotalLenError,
        }
        public inout InOut;                                     // 输入输出参数结构体

        public strerror Str2Data()
        {
            string PATTERN = @"([^A-Fa-f0-9]|\s+?)+";
            char[] chs = { ' ' };
            string[] sData = textBox_Send.Text.Split(chs, options: StringSplitOptions.RemoveEmptyEntries);
            if (sData.Length == 0)
            {
                InOut.InputLen = 0;
                return strerror.None;
            }
            if (sData.Length > 256)
            {
                InOut.InputLen = 0;
                return strerror.TotalLenError;
            }
            for (uint i = 0; i < sData.Length; i++)
            {
                if (sData[i].Length != 2)
                    return strerror.LenError;
                if (System.Text.RegularExpressions.Regex.IsMatch(sData[i], PATTERN))
                    return strerror.CharError;
            }
            for (uint i = 0; i < sData.Length; i++)
            {
                InOut.InputData[i] = Convert.ToByte(sData[i], 16);
            }
            InOut.InputLen = (uint)sData.Length;
            return strerror.None;
        }


        public void ModbusConv()
        {
            Array.Copy(InOut.InputData, InOut.ConvData, InOut.InputLen);
            InOut.ConvLen = InOut.InputLen;
        }


        public class ModBus {
            public static uint GetCRC(byte[] Data, uint DataLen)
            {
                uint CRCLo = 0xFF;                                  // 高CRC字节初始化
                uint CRCHi = 0xFF;                                  // 低CRC字节初始化 
                uint uIndex = 0;                                    // CRC循环中的索引 

                for (uint i = 0; i < DataLen; i++)                  // 传输消息缓冲区
                {
                    uIndex = CRCLo ^ Data[i];                       // 计算CRC 
                    CRCLo = CRCHi ^ auchCRCLo[uIndex];
                    CRCHi = auchCRCHi[uIndex];
                }

                return (CRCHi << 8) | CRCLo;   //高  低
            }

            public static byte[] GetRTU(byte[] RtuData, uint DataLen)
            {
                byte[] NewRtuData = new byte[DataLen + 2];
                for (int i = 0; i < DataLen; i++)
                {
                    NewRtuData[i] = RtuData[i];
                }

                uint CRC = GetCRC(RtuData, DataLen);
                NewRtuData[DataLen] = (byte)CRC;
                NewRtuData[DataLen + 1] = (byte)(CRC >> 8);  //低 高
                return NewRtuData;

            }
            static byte[] auchCRCLo = { 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
                                        0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
                                        0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
                                        0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
                                        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
                                        0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
                                        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
                                        0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
                                        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
                                        0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
                                        0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
                                        0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
                                        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
                                        0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
                                        0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
                                        0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
                                        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
                                        0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
                                        0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
                                        0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
                                        0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
                                        0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
                                        0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
                                        0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
                                        0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
                                        0x80, 0x41, 0x00, 0xC1, 0x81, 0x40 };
            /* CRC高位字节值表*/
            static byte[] auchCRCHi = { 0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
                                        0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
                                        0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
                                        0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
                                        0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
                                        0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
                                        0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
                                        0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
                                        0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
                                        0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
                                        0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
                                        0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
                                        0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
                                        0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
                                        0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
                                        0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
                                        0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
                                        0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
                                        0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
                                        0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
                                        0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
                                        0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
                                        0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
                                        0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
                                        0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
                                        0x43, 0x83, 0x41, 0x81, 0x80, 0x40 };
        }
        private void button8_Click(object sender, EventArgs e)
        {
            try
            {
                if (serialPort1.IsOpen)  //串口已打开
                {
                    serialPort1.Close();
                    button8.Text = "打开串口";
                    button8.BackColor = Color.ForestGreen;
                    comboBox1.Enabled = true;
                    comboBox2.Enabled = true;
                    comboBox3.Enabled = true;
                    comboBox4.Enabled = true;
                    comboBox5.Enabled = true;
                    textBox_Send.Text = "";
                    textBox_Receive.Text = "";
                }
                else
                {
                    comboBox1.Enabled = false;
                    comboBox2.Enabled = false;
                    comboBox3.Enabled = false;
                    comboBox4.Enabled = false;
                    comboBox5.Enabled = false;
                    serialPort1.PortName = comboBox1.Text;
                    serialPort1.BaudRate = Convert.ToInt32(comboBox2.Text);
                    serialPort1.DataBits = Convert.ToInt32(comboBox3.Text);
                    if (comboBox4.Text.Equals("None"))
                    {
                        serialPort1.Parity = System.IO.Ports.Parity.None;
                    }
                    if (comboBox5.Text.Equals("1"))
                    {
                        serialPort1.StopBits = System.IO.Ports.StopBits.One;
                    }

                    serialPort1.Open();
                    button8.Text = "关闭串口";
                    button8.BackColor = Color.Firebrick;

                }

            }
            catch (Exception ex)
            {
                serialPort1 = new System.IO.Ports.SerialPort();

                comboBox1.Items.Clear();
                comboBox1.Items.AddRange(System.IO.Ports.SerialPort.GetPortNames());
                System.Media.SystemSounds.Beep.Play();
                button8.Text = "打开串口";
                button8.BackColor = Color.ForestGreen;
                MessageBox.Show(ex.Message);
                comboBox1.Enabled = true;
                comboBox2.Enabled = true;
                comboBox3.Enabled = true;
                comboBox4.Enabled = true;
                comboBox5.Enabled = true;
            }
            finally
            {

            }
        }   //open serials

        private void button9_Click(object sender, EventArgs e)
        {
            byte[] temp = new byte[1];
            try
            {
                if (serialPort1.IsOpen)
                {

                    int num = 0;
                    string sendbuff = textBox_Send.Text;
                    string pattern = @"\s";
                    string replacement = "";
                    Regex rgx = new Regex(pattern);
                    string Send_Data = rgx.Replace(sendbuff, replacement);

                    num = (Send_Data.Length - Send_Data.Length % 2) / 2;
                    for (int i = 0; i < num; i++)
                    {
                        temp[0] = Convert.ToByte(Send_Data.Substring(i * 2, 2), 16);
                        serialPort1.Write(temp, 0, 1);
                    } //00 01 这类



                }
            }
            catch (Exception ex)
            {
                serialPort1 = new System.IO.Ports.SerialPort();

                comboBox1.Items.Clear();
                comboBox1.Items.AddRange(System.IO.Ports.SerialPort.GetPortNames());
                System.Media.SystemSounds.Beep.Play();
                button8.Text = "打开串口";
                button8.BackColor = Color.ForestGreen;
                MessageBox.Show(ex.Message);
                comboBox1.Enabled = true;
                comboBox2.Enabled = true;
                comboBox3.Enabled = true;
                comboBox4.Enabled = true;
                comboBox5.Enabled = true;
            }
            finally
            {
            }

        }

        private void serialPort1_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            System.Threading.Thread.Sleep(300);
            int num = serialPort1.BytesToRead;
            byte[] receive_buff = new byte[num];
            serialPort1.Read(receive_buff, 0, num);
            funccode.Clear();
            recbuf.Clear();
            foreach (byte b in receive_buff)
            {
                recbuf.Append(b.ToString("X2") + ' ');
                funccode.Append(b);
            }
            System.Threading.Thread.Sleep(100);
            int recnum = receive_buff.Length;
            //startRegAddr = (((uint)receive_buff[2] << 8) | receive_buff[3]);
           uint crccal = ModBus.GetCRC(receive_buff, (uint)recnum - 2);
            uint crcget = ((uint)receive_buff[recnum - 1] << 8) | receive_buff[recnum - 2];
            if ((crccal == crcget)&&(receive_buff[0]==0x01))
            {
                DealWithFunc(receive_buff, recnum);
            }
            try
            {
                this.Invoke((EventHandler)(delegate
                    {
                        textBox_Receive.AppendText(recbuf.ToString());
                    })
                    );
               
            }
            catch (Exception ex)
            {
                System.Media.SystemSounds.Beep.Play();
                MessageBox.Show(ex.Message);
            }
            finally
            {
            }


        }

        public void DealWithFunc(byte[] buff, int length)
        {

            switch (buff[1])
            {
                case 0x01: Modbus_01_Solve(buff,length); break;
                case 0x02: Modbus_02_Solve(buff,length); break;
                case 0x03: Modbus_03_Solve(buff,length); break;
                case 0x06: Modbus_06_Solve(buff,length); break;
            }

        }
        public int GetBit(int input, int index)
        {
            
            return (input>>index)&1;
        }
        private void Modbus_01_Solve(byte[] buff, int length)
        {
            b10 = GetBit(buff[3], 0);
            b11 = GetBit(buff[3], 1);
            b12 = GetBit(buff[3], 2);
            b13 = GetBit(buff[3], 3);
            b14 = GetBit(buff[3], 4);
            if (b10 == 0)
            { 
                button1.BackColor = Color.ForestGreen;
                this.Invoke((EventHandler)(delegate
                {
                    checkBox1.Checked = true;
                })
                    );

            }
            if (b10 == 1)
            {
                button1.BackColor = Color.Empty;
                
                this.Invoke((EventHandler)(delegate
                {
                    checkBox1.Checked = false;
                })
                    );
            }
            if (b11 == 0)
            {
                button2.BackColor = Color.ForestGreen;
                this.Invoke((EventHandler)(delegate
                {
                    checkBox2.Checked = true;
                })
                    );
            }
            if (b11 == 1)
            {
                button2.BackColor = Color.Empty;
                this.Invoke((EventHandler)(delegate
                {
                    checkBox2.Checked = false;
                })
                    );
            }
            if (b12 == 0)
            {
                button3.BackColor = Color.ForestGreen;
                this.Invoke((EventHandler)(delegate
                {
                    checkBox3.Checked = true;
                })
                    );
            }
            if(b12 == 1)
            {
                button3.BackColor = Color.Empty;
                this.Invoke((EventHandler)(delegate
                {
                    checkBox3.Checked = false;
                })
                    );
            }
            if (b13 == 0)
            {
                button4.BackColor = Color.ForestGreen;
                this.Invoke((EventHandler)(delegate
                {
                    checkBox4.Checked = true;
                })
                    );
            }
            if (b13 == 1)
            {
                button4.BackColor = Color.Empty;
                this.Invoke((EventHandler)(delegate
                {
                    checkBox4.Checked = false;
                })
                    );
            }
            if (b14 == 0)
            {
                button5.BackColor = Color.ForestGreen;
                this.Invoke((EventHandler)(delegate
                {
                    checkBox5.Checked = true;
                })
                    );
            }
            if (b14 == 1)
            {
                button5.BackColor = Color.Empty;
                this.Invoke((EventHandler)(delegate
                {
                    checkBox5.Checked = false;
                })
                    );
            }
        }

        private void Modbus_02_Solve(byte[] buff, int length)
        {
             b20 = GetBit(buff[3], 0);
             b21 = GetBit(buff[3], 1);
            if (b20 == 0)
            {
                button6.BackColor = Color.ForestGreen;

            }
            if (b20 == 1)
            {
                button6.BackColor = Color.Empty;
            }
            if (b21 == 0)
            {
                button7.BackColor = Color.ForestGreen;
            }
            if (b21 == 1)
            {
                button7.BackColor = Color.Empty;
            }
        }
        private void Modbus_03_Solve(byte[] buff, int length)
        {
            uint ADC_SHOW = ((uint)buff[3] << 8) | (buff[4]);
            double adc_vol=ADC_SHOW*3.3 / 4096;
            
            this.Invoke((EventHandler)(delegate
            {
                textBox2.Text = System.Math.Round(adc_vol, 3).ToString();
            })
                    );
        }
        private void Modbus_06_Solve(byte[] buff, int length)
        {

        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)//checked or !checked
        { 
            if (checkBox1.Checked)
            {
                byte[] temp = new byte[1];
                button1.BackColor = Color.ForestGreen;
                string sendbuff = "01050000ff008c3a";

                int num = (sendbuff.Length - sendbuff.Length % 2) / 2;
                for (int i = 0; i < num; i++)
                {
                    temp[0] = Convert.ToByte(sendbuff.Substring(i * 2, 2), 16);
                    serialPort1.Write(temp, 0, 1);
                } //00 01 这类

            }
            else
            {
                button1.BackColor = Color.Empty;
                byte[] temp = new byte[1];
                string sendbuff = "010500000000cdca";

                int num = (sendbuff.Length - sendbuff.Length % 2) / 2;
                for (int i = 0; i < num; i++)
                {
                    temp[0] = Convert.ToByte(sendbuff.Substring(i * 2, 2), 16);
                    serialPort1.Write(temp, 0, 1);
                } //00 01 这类
            }

        }

        private void checkBox2_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox2.Checked)
            {
                byte[] temp = new byte[1];
                button2.BackColor = Color.ForestGreen;
                string sendbuff = "01050001FF00DDFA";

                int num = (sendbuff.Length - sendbuff.Length % 2) / 2;
                for (int i = 0; i < num; i++)
                {
                    temp[0] = Convert.ToByte(sendbuff.Substring(i * 2, 2), 16);
                    serialPort1.Write(temp, 0, 1);
                } //00 01 这类

            }
            else
            {
                button2.BackColor = Color.Empty;
                byte[] temp = new byte[1];
                string sendbuff = "0105000100009C0A ";

                int num = (sendbuff.Length - sendbuff.Length % 2) / 2;
                for (int i = 0; i < num; i++)
                {
                    temp[0] = Convert.ToByte(sendbuff.Substring(i * 2, 2), 16);
                    serialPort1.Write(temp, 0, 1);
                } //00 01 这类
            }
        }

        private void checkBox3_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox3.Checked)
            {
                byte[] temp = new byte[1];
                button3.BackColor = Color.ForestGreen;
                string sendbuff = "01050002FF002DFA";
 

                int num = (sendbuff.Length - sendbuff.Length % 2) / 2;
                for (int i = 0; i < num; i++)
                {
                    temp[0] = Convert.ToByte(sendbuff.Substring(i * 2, 2), 16);
                    serialPort1.Write(temp, 0, 1);
                } //00 01 这类

            }
            else
            {
                button3.BackColor = Color.Empty;
                byte[] temp = new byte[1];
                string sendbuff = "0105000200006C0A";

                int num = (sendbuff.Length - sendbuff.Length % 2) / 2;
                for (int i = 0; i < num; i++)
                {
                    temp[0] = Convert.ToByte(sendbuff.Substring(i * 2, 2), 16);
                    serialPort1.Write(temp, 0, 1);
                } //00 01 这类
            }
        }

        private void checkBox4_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox4.Checked)
            {
                byte[] temp = new byte[1];
                button4.BackColor = Color.ForestGreen;
                string sendbuff = "01050003FF007C3A";


                int num = (sendbuff.Length - sendbuff.Length % 2) / 2;
                for (int i = 0; i < num; i++)
                {
                    temp[0] = Convert.ToByte(sendbuff.Substring(i * 2, 2), 16);
                    serialPort1.Write(temp, 0, 1);
                } //00 01 这类

            }
            else
            {
                button4.BackColor = Color.Empty;
                byte[] temp = new byte[1];
                string sendbuff = "0105000300003DCA";

                int num = (sendbuff.Length - sendbuff.Length % 2) / 2;
                for (int i = 0; i < num; i++)
                {
                    temp[0] = Convert.ToByte(sendbuff.Substring(i * 2, 2), 16);
                    serialPort1.Write(temp, 0, 1);
                } //00 01 这类
            }
        }

        private void checkBox5_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox5.Checked)
            {
                byte[] temp = new byte[1];
                button5.BackColor = Color.ForestGreen;
                string sendbuff = "01050004FF00CDFB";


                int num = (sendbuff.Length - sendbuff.Length % 2) / 2;
                for (int i = 0; i < num; i++)
                {
                    temp[0] = Convert.ToByte(sendbuff.Substring(i * 2, 2), 16);
                    serialPort1.Write(temp, 0, 1);
                } //00 01 这类

            }
            else
            {
                button5.BackColor = Color.Empty;
                byte[] temp = new byte[1];
                string sendbuff = "0105000400008C0B";

                int num = (sendbuff.Length - sendbuff.Length % 2) / 2;
                for (int i = 0; i < num; i++)
                {
                    temp[0] = Convert.ToByte(sendbuff.Substring(i * 2, 2), 16);
                    serialPort1.Write(temp, 0, 1);
                } //00 01 这类
            }
        }

        private void button10_Click(object sender, EventArgs e)
        {
            byte[] temp = new byte[1];
            string sendbuff = "010100000005FC09";


            int num = (sendbuff.Length - sendbuff.Length % 2) / 2;
            for (int i = 0; i < num; i++)
            {
                temp[0] = Convert.ToByte(sendbuff.Substring(i * 2, 2), 16);
                serialPort1.Write(temp, 0, 1);
            } //00 01 这类
        }
        private void button11_Click(object sender, EventArgs e)
        {
            byte[] temp = new byte[1];
            string sendbuff = "010200000002F9CB";

            int num = (sendbuff.Length - sendbuff.Length % 2) / 2;
            for (int i = 0; i < num; i++)
            {
                temp[0] = Convert.ToByte(sendbuff.Substring(i * 2, 2), 16);
                serialPort1.Write(temp, 0, 1);
            } //00 01 这类
        }

        private void textBox1_KeyPress(object sender, KeyPressEventArgs e)
        {
            e.Handled = true;
            if ((e.KeyChar >= 48 && e.KeyChar <= 58)||e.KeyChar==46||e.KeyChar==8)
            {
                e.Handled=false;
            }
            if ((e.KeyChar == 13)&&(serialPort1.IsOpen))
            {
                try {
                    float a = float.Parse(textBox1.Text);
                   
                    int dac = (int)a * 1000;
                    string mes = string.Format("adc:{0}", dac);
                }
                catch ( Exception ec)
                {
                    MessageBox.Show(ec.Message);
                }
            
            }
        }


        private void button12_Click(object sender, EventArgs e)
        {
            byte[] temp = new byte[1];
            string sendbuff = "01030000000305CB";

            int num = (sendbuff.Length - sendbuff.Length % 2) / 2;
            for (int i = 0; i < num; i++)
            {
                temp[0] = Convert.ToByte(sendbuff.Substring(i * 2, 2), 16);
                serialPort1.Write(temp, 0, 1);
            } //00 01 这类
        }

        private void button13_Click(object sender, EventArgs e)
        {
            byte[] temp = new byte[1];
            float a = float.Parse(textBox1.Text);
            int dac_set = (int)( a * 3300 / 3.3);
            string dac_s = dac_set.ToString("X4");

            //DAC.Clear();
            //foreach (byte b in dac_s)
            //{
            //    recbuf.Append(b.ToString("X2") + ' ');
            //    funccode.Append(b);
            //}
            string sendbuff = "01060001" + dac_s + "DF48";

            //MessageBox.Show(sendbuff);
            int num = (sendbuff.Length - sendbuff.Length % 2) / 2;
            for (int i = 0; i < num; i++)
            {
                temp[0] = Convert.ToByte(sendbuff.Substring(i * 2, 2), 16);
                serialPort1.Write(temp, 0, 1);
            } //00 01 这类

        }
    }
}
