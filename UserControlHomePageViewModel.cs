using Prism.Commands;
using Quartz;
using Quartz.Impl;
using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.ComponentModel;
using System.IO.Ports;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Threading;
using VocGas.App.Helper;
using VocGas.App.Jobs;
using VocGas.App.Model;
using VocGas.App.Model.HomePage;
using VocGas.App.View.HomePage;
using VocGas.App.View.ParaSettingView;
using VocGas.Common.Command;
using VocGas.Common.Helper;
using VocGas.EFCore.DB;
using VocGas.EFCore.Entitys;

namespace VocGas.App.ViewModel.HomePage
{
    public class UserControlHomePageViewModel : NotifyPropertyBase
    {
        #region 变量
        private UserControlHomePage _userControlHomePage;//当前窗口
        private bool processing = false;
        BackgroundWorker bw = new BackgroundWorker();
        private ISchedulerFactory _schedulerFactory;
        private StdSchedulerFactory factory = new StdSchedulerFactory();
        private IScheduler _scheduler;

        private static System.Windows.Threading.DispatcherTimer loadFactorInfoTimer = new System.Windows.Threading.DispatcherTimer();
        public System.Windows.Threading.DispatcherTimer handleModbusPlc212Timer = new System.Windows.Threading.DispatcherTimer();
        private static System.Windows.Threading.DispatcherTimer updateDataTimer = new System.Windows.Threading.DispatcherTimer();

        private System.ComponentModel.IContainer components = null;//必需的设计器变量

        public System.IO.Ports.SerialPort spModbus = new SerialPort();//色谱仪端口
        public System.IO.Ports.SerialPort spPLC = new SerialPort();//PLC端口
        public System.IO.Ports.SerialPort spHJ212 = new SerialPort();//HJ212端口
        public System.IO.Ports.SerialPort spHJ212_2 = new SerialPort();//HJ212_2端口

        public String[] factorsNames = new String[100];//因子名称
        public String[] factorsUnits = new String[100];//因子单位
        public String[] factorsValues = new String[100];//因子数据
        public String[] factorsStates = new String[100];//因子读取状态
        public String[] showFactorNames = new String[30];//需要显示和上传的因子

        public int[] locY = new int[] { 173, 203, 233, 263, 293, 323, 353, 383, 413, 443, 473, 503, 533, 563, 593, 623, 653, 683, 683, 683, 683 };//panel的显示位置

        int overTimes = 2;//超时次数
        ToolsHepler toolHelper = new ToolsHepler();//数据获取帮助类

        private int[] gp = new int[100];//创建数组存放每个模块的输入通道号，之后将会根据不同的通道号读取对应的数据
        private float[] lc = new float[100];//创建一个存放量程差的数组
        private float[] minlc = new float[100];//存放最低量程
        private float[] maxlc = new float[100];//存放最高量程

        private static readonly object timerLock = new object();//线程开启条件，进程锁
        private static readonly object timerUpdateLock = new object();
        private static readonly object timerLoadFactorLock = new object();

        bool caliFlag = true;//校准模式  开启时不读取色谱仪数据
        string flag212_1 = "N", flag212_2 = "N";//212协议因子标志 1是色谱仪  2是PLC或其他模块
        public bool tapflag = true;//PLC读取状态标志
        public int timeCYB = 180, timeFC = 8;//PLC读取采样泵和反吹时间
        public bool ifReadPLC = true;//是否读取PLC

        bool hjtextshow = false;
        public decimal flueGasHumidityValue;//烟气湿度数值
        public bool flueGasHumidityChecked;//烟气湿度是否被选中
        public int testNumber = 0;//测试数据传递
        private BasSysFixedParaSetting fixParaSetting;//系统设置参数
        private List<BasFactorEquSettingModel> equInfos = new List<BasFactorEquSettingModel>();//读取设备端口设置信息集合
        private List<BssComSettingModel> comSettingInfos = new List<BssComSettingModel>(); //读取端口号和波特率信息集合

        //读取端口号和波特率信息
        private string vocType = string.Empty;//色谱仪协议类型
        private string PLCType = string.Empty;//PLC协议类型
        private string HJ212Type = string.Empty;//国际协议类型
        private string HJ212Type_2 = string.Empty;//国际协议类型2

        private int maxTimes212 = 0;//记录失败次数
        #endregion

        #region 封装字段
        /// <summary>
        /// 首页项目显示表
        /// </summary>
        private List<BssFactorModel> _homePageProjectList;

        //public IList<BssFactorModel> HomePageProjectList { get => _homePageProjectList; set => _homePageProjectList = value; }
        public List<BssFactorModel> HomePageProjectList
        {
            get { return _homePageProjectList; }
            set
            {
                _homePageProjectList = value;
                OnPropertyChanged("HomePageProjectList");
            }
        }

        #region 控制显示

        /// <summary>
        /// 清空数据按钮是否显示
        /// </summary>
        private Visibility _visClearData;
        public Visibility VisClearData
        {
            get { return _visClearData; }
            set
            {
                _visClearData = value;
                OnPropertyChanged("VisClearData");
            }
        }

        /// <summary>
        /// 参数设置按钮是否显示
        /// </summary>
        private Visibility _visParaSetting;
        public Visibility VisParaSetting
        {
            get { return _visParaSetting; }
            set
            {
                _visParaSetting = value;
                OnPropertyChanged("VisParaSetting");
            }
        }

        /// <summary>
        /// 是否显示FID-A
        /// </summary>
        private Visibility _visFIDA;
        public Visibility VisFIDA
        {
            get { return _visFIDA; }
            set
            {
                _visFIDA = value;
                OnPropertyChanged("VisFIDA");
            }
        }

        /// <summary>
        /// 是否显示FID-B
        /// </summary>
        private Visibility _visFIDB;
        public Visibility VisFIDB
        {
            get { return _visFIDB; }
            set
            {
                _visFIDB = value;
                OnPropertyChanged("VisFIDB");
            }
        }

        /// <summary>
        /// 是否显示色谱柱实时温度
        /// </summary>
        private Visibility _visChromatographicColumn;
        public Visibility VisChromatographicColumn
        {
            get { return _visChromatographicColumn; }
            set
            {
                _visChromatographicColumn = value;
                OnPropertyChanged("VisChromatographicColumn");
            }
        }

        /// <summary>
        /// 是否允许查看已完成次数及仪器状态
        /// </summary>
        private Visibility _visCompleteInstrumentStatus;
        public Visibility VisCompleteInstrumentStatus
        {
            get { return _visCompleteInstrumentStatus; }
            set
            {
                _visCompleteInstrumentStatus = value;
                OnPropertyChanged("VisCompleteInstrumentStatus");
            }
        }
        #endregion

        #region 4个按钮接收数据
        /// <summary>
        /// FID-A接收数据
        /// </summary>
        private string _tbFIDA;
        public string TbFIDA
        {
            get { return _tbFIDA; }
            set
            {
                _tbFIDA = value;
                OnPropertyChanged("TbFIDA");
            }
        }

        /// <summary>
        /// FID-B接收数据
        /// </summary>
        private string _tbFIDB;
        public string TbFIDB
        {
            get { return _tbFIDB; }
            set
            {
                _tbFIDB = value;
                OnPropertyChanged("TbFIDB");
            }
        }

        /// <summary>
        /// 色谱柱实时温度接收数据
        /// </summary>
        private string _tbChromatographicColumn;
        public string TbChromatographicColumn
        {
            get { return _tbChromatographicColumn; }
            set
            {
                _tbChromatographicColumn = value;
                OnPropertyChanged("TbChromatographicColumn");
            }
        }

        /// <summary>
        ///是否允许查看已完成次数及仪器状态接收数据
        /// </summary>
        private string _tbCompleteInstrumentStatus;
        public string TbCompleteInstrumentStatus
        {
            get { return _tbCompleteInstrumentStatus; }
            set
            {
                _tbCompleteInstrumentStatus = value;
                OnPropertyChanged("TbCompleteInstrumentStatus");
            }
        }
        #endregion

        /// <summary>
        /// 校准模式按钮文字
        /// </summary>
        private string _calibrateContent;
        public string CalibrateContent
        {
            get { return _calibrateContent; }
            set
            {
                _calibrateContent = value;
                OnPropertyChanged("CalibrateContent");
            }
        }

        ////旧方式写法
        //public string UserList
        //{
        //    get
        //    {
        //        return _userList;
        //    }
        //    set
        //    {
        //        _userList = value;
        //        OnPropertyChanged("UserList");
        //    }
        //}
        #endregion

        #region 委托命令
        /// <summary>
        /// 数据清空
        /// </summary>
        private DelegateCommand _cmdClearData;

        public DelegateCommand CmdClearData { get => _cmdClearData; set => _cmdClearData = value; }

        /// <summary>
        /// 校准模式
        /// </summary>
        private DelegateCommand _cmdCalibrate;

        public DelegateCommand CmdCalibrate { get => _cmdCalibrate; set => _cmdCalibrate = value; }

        /// <summary>
        /// 参数设置
        /// </summary>
        private DelegateCommand _cmdParaSetting;
        public DelegateCommand CmdParaSetting { get => _cmdParaSetting; set => _cmdParaSetting = value; }

        #region 4个指令按钮
        /// <summary>
        /// FIDA按钮
        /// </summary>
        private DelegateCommand _cmdFIDA;
        public DelegateCommand CmdFIDA { get => _cmdFIDA; set => _cmdFIDA = value; }

        /// <summary>
        /// FIDB按钮
        /// </summary>
        private DelegateCommand _cmdFIDB;
        public DelegateCommand CmdFIDB { get => _cmdFIDB; set => _cmdFIDB = value; }

        /// <summary>
        /// 色谱柱实时温度按钮
        /// </summary>
        private DelegateCommand _cmdChromatographicColumn;
        public DelegateCommand CmdChromatographicColumn { get => _cmdChromatographicColumn; set => _cmdChromatographicColumn = value; }

        /// <summary>
        /// 已完成次数及仪器状态按钮
        /// </summary>
        private DelegateCommand _cmdCompleteInstrumentStatus;
        public DelegateCommand CmdCompleteInstrumentStatus { get => _cmdCompleteInstrumentStatus; set => _cmdCompleteInstrumentStatus = value; }

        #endregion
        #endregion

        #region 初加载绑定命令
        public UserControlHomePageViewModel(UserControlHomePage userControlHomePage)
        {
            //bw.DoWork += bw_DoWork;
            //bw.WorkerSupportsCancellation = true;
            //bw.RunWorkerCompleted += bw_RunWorkerCompleted;
            _userControlHomePage = userControlHomePage;

            //加载数据
            LoadHomePageProjectList();

            //命令绑定
            _cmdClearData = new DelegateCommand(ClearData, CanExcute);
            _cmdCalibrate = new DelegateCommand(CalibrateAsync, CanExcute);

            _cmdParaSetting = new DelegateCommand(ParaSeting, CanExcute);

            //4个按钮指令按钮
            _cmdFIDA = new DelegateCommand(FIDA, CanExcute);
            _cmdFIDB = new DelegateCommand(FIDB, CanExcute);
            _cmdChromatographicColumn = new DelegateCommand(ChromatographicColumn, CanExcute);
            _cmdCompleteInstrumentStatus = new DelegateCommand(CompleteInstrumentStatus, CanExcute);
        }

        private void LoadCom()
        {
            //this.components = new Container();
            //ComponentResourceManager resources = new ComponentResourceManager(typeof(UserControlHomePageViewModel));
            //this.spModbus = new System.IO.Ports.SerialPort(this.components);
            //this.spPLC = new System.IO.Ports.SerialPort(this.components);
            //this.spHJ212 = new System.IO.Ports.SerialPort(this.components);
            //this.spHJ212_2 = new System.IO.Ports.SerialPort(this.components);

            //调试1
            //try
            //{
            //    spPLC = new SerialPort();
            //    SerialPortUtils.OpenClosePort(spPLC, "COM2", 9600, 8);
            //}
            //catch (Exception ex)
            //{
            //    LogHelper.Error($"调试出错", ex);
            //}

            try
            {
                LogHelper.Info($"尝试初始化各个接口");

                //调试2
                //spModbus = new SerialPort("COM4", 9600, Parity.None, 8, StopBits.One);
                //spModbus.Open();

                //spPLC = new SerialPort("COM2", 9600, Parity.None, 8, StopBits.One);
                //spPLC.Open();

                //spHJ212 = new SerialPort("COM3", 9600, Parity.None, 8, StopBits.One);
                //spHJ212.Open();

                //spHJ212_2 = new SerialPort("COM6", 9600, Parity.None, 8, StopBits.One);
                //spHJ212_2.Open();

                spModbus = new SerialPort();

                spPLC = new SerialPort();

                spHJ212 = new SerialPort();

                spHJ212_2 = new SerialPort();

                ////从ComSetting表中循环初始化各个接口
                //LoadComsFromSetting();
            }
            catch (Exception ex)
            {
                LogHelper.Error($"尝试初始化各个接口失败", ex);
            }

        }

        /// <summary>
        /// 从ComSetting表中循环初始化各个接口
        /// </summary>
        private void LoadComsFromSetting()
        {
            comSettingInfos.ForEach(p =>
            {
                switch (p.ComTarget)
                {
                    case "ModBus":
                        spModbus.Close();
                        spModbus.PortName = p.ComNumber;//端口号
                        spModbus.BaudRate = p.BaudRate;//波特率

                        spModbus.DataBits = p.DataBit ?? 8;
                        spModbus.DiscardNull = false;
                        spModbus.DtrEnable = false;
                        spModbus.Handshake = Handshake.None;
                        spModbus.Parity = Parity.None;
                        spModbus.ParityReplace = 63;

                        spModbus.ReadBufferSize = 4096;
                        spModbus.ReadTimeout = -1;
                        spModbus.ReceivedBytesThreshold = 1;
                        spModbus.RtsEnable = false;
                        spModbus.StopBits = StopBits.One;
                        spModbus.ReadBufferSize = 2048;
                        spModbus.WriteTimeout = -1;

                        vocType = p.ComType;//类型
                        break;
                    case "PLC":
                        spPLC.Close();
                        spPLC.PortName = p.ComNumber;//端口号
                        spPLC.BaudRate = p.BaudRate;//波特率

                        spPLC.DataBits = p.DataBit ?? 8;
                        spPLC.DiscardNull = false;
                        spPLC.DtrEnable = false;
                        spPLC.Handshake = Handshake.None;
                        spPLC.Parity = Parity.None;
                        spPLC.ParityReplace = 63;

                        spPLC.ReadBufferSize = 4096;
                        spPLC.ReadTimeout = -1;
                        spPLC.ReceivedBytesThreshold = 1;
                        spPLC.RtsEnable = false;
                        spPLC.StopBits = StopBits.One;
                        spPLC.ReadBufferSize = 2048;
                        spPLC.WriteTimeout = -1;

                        PLCType = p.ComType;//类型
                        break;
                    case "HJ212":
                        spHJ212.Close();
                        spHJ212.PortName = p.ComNumber;//端口号
                        spHJ212.BaudRate = p.BaudRate;//波特率

                        spHJ212.DataBits = p.DataBit ?? 8;
                        spHJ212.DiscardNull = false;
                        spHJ212.DtrEnable = false;
                        spHJ212.Handshake = Handshake.None;
                        spHJ212.Parity = Parity.None;
                        spHJ212.ParityReplace = 63;

                        spHJ212.ReadBufferSize = 4096;
                        spHJ212.ReadTimeout = -1;
                        spHJ212.ReceivedBytesThreshold = 1;
                        spHJ212.RtsEnable = false;
                        spHJ212.StopBits = StopBits.One;
                        spHJ212.ReadBufferSize = 2048;
                        spHJ212.WriteTimeout = -1;

                        HJ212Type = p.ComType;//类型
                        break;
                    case "HJ212-2":
                        spHJ212_2.Close();
                        spHJ212_2.PortName = p.ComNumber;//端口号
                        spHJ212_2.BaudRate = p.BaudRate;//波特率

                        spHJ212_2.DataBits = p.DataBit ?? 8;
                        spHJ212_2.DiscardNull = false;
                        spHJ212_2.DtrEnable = false;
                        spHJ212_2.Handshake = Handshake.None;
                        spHJ212_2.Parity = Parity.None;
                        spHJ212_2.ParityReplace = 63;

                        spHJ212_2.ReadBufferSize = 4096;
                        spHJ212_2.ReadTimeout = -1;
                        spHJ212_2.ReceivedBytesThreshold = 1;
                        spHJ212_2.RtsEnable = false;
                        spHJ212_2.StopBits = StopBits.One;
                        spHJ212_2.ReadBufferSize = 2048;
                        spHJ212_2.WriteTimeout = -1;

                        HJ212Type_2 = p.ComType;//类型    
                        break;
                    case "天蓝":
                        spModbus.Close();
                        spModbus.PortName = p.ComNumber;//端口号
                        spModbus.BaudRate = p.BaudRate;//波特率

                        spModbus.DataBits = p.DataBit ?? 8;
                        spModbus.DiscardNull = false;
                        spModbus.DtrEnable = false;
                        spModbus.Handshake = Handshake.None;
                        spModbus.Parity = Parity.None;
                        spModbus.ParityReplace = 63;

                        spModbus.ReadBufferSize = 4096;
                        spModbus.ReadTimeout = -1;
                        spModbus.ReceivedBytesThreshold = 1;
                        spModbus.RtsEnable = false;
                        spModbus.StopBits = StopBits.One;
                        spModbus.ReadBufferSize = 2048;
                        spModbus.WriteTimeout = -1;

                        vocType = p.ComType;//类型
                        break;
                }
            });
        }

        #endregion

        #region 委托绑定所用的方法参数
        #region 数据初加载
        /// <summary>
        /// 加载下来用户数据集合
        /// </summary>
        /// <returns></returns>
        public void LoadHomePageProjectList()
        {
            CalibrateContent = "校准模式";

            #region 控制显隐
            VisClearData = GlobalVariables.VisClearData;
            VisParaSetting = GlobalVariables.VisParaSetting;

            VisFIDA = GlobalVariables.VisFIDA;
            VisFIDB = GlobalVariables.VisFIDB;
            VisChromatographicColumn = GlobalVariables.VisChromatographicColumn;
            VisCompleteInstrumentStatus = GlobalVariables.VisCompleteInstrumentStatus;
            #endregion

            using (var db = new DatabaseContext())
            {
                var result = new List<BssFactorModel>();
                try
                {
                    //var dateNow = DateTime.Now;
                    //result = db.BssFactors.Where(p => p.Deleted == false
                    // && p.CreateTime.Year == dateNow.Year && p.CreateTime.Month == dateNow.Month && p.CreateTime.Day == dateNow.Day).ToList()
                    //     .Select(p => new BssFactorModel
                    //     {
                    //         FactorName = p.FactorName,
                    //         FactorValue = p.FactorValue.ToString(),
                    //         FactorUnit = p.FactorUnit,
                    //         FactorState = GetProjectState(p.FactorState)
                    //     }).ToList();

                    //if (result == null || result.Count == 0)
                    //{
                    //    //显示默认初始数据
                    //    ShowDefaultFactorData(db, ref result);
                    //}

                    ////数据折算处理逻辑
                    //FactorDataHandle(result);
                    //加载数据
                    LogHelper.Info($"加载首页因子数据");
                    LoadInfoes(db);

                    //显示默认初始数据
                    ShowDefaultFactorData(db, ref result);
                }
                catch (Exception ex)
                {
                    MessageBox.Show($"数据加载错误，错误信息：{ex}");
                    LogHelper.Error($"数据加载错误", ex);
                    //显示默认初始数据
                    ShowDefaultFactorData(db, ref result);
                }

                showFactorNames = result.Select(p => p.FactorName).ToArray();

                HomePageProjectList = result;
            }

            //在加载完数据之后，初始化串口
            LoadCom();

            //定时读取数据
            LogHelper.Info($"开启定时器");
            TimerLoad();
        }

        /// <summary>
        /// 加载数据
        /// </summary>
        /// <param name="db"></param>
        private void LoadInfoes(DatabaseContext db)
        {
            LogHelper.Info($"读取系统设置参数，加载烟气湿度数据");
            fixParaSetting = db.BasSysFixedParaSettings.Where(p => p.Deleted == false).FirstOrDefault();
            flueGasHumidityChecked = fixParaSetting.FlueGasHumidityChecked;
            flueGasHumidityValue = fixParaSetting.FlueGasHumidity ?? 0;

            //读取设备端口设置信息集合
            equInfos = db.BasFactorEquSettings.Where(p => p.Deleted == false).OrderBy(p => p.Id)
                .Select(p => new BasFactorEquSettingModel
                {
                    Id = p.Id,
                    ElectricalRangeBegin = p.ElectricalRangeBegin,
                    ElectricalRangeEnd = p.ElectricalRangeEnd,
                    ModuleChannelIn = p.ModuleChannelIn
                }).ToList();

            for (int i = 0; i < equInfos.Count; i++)
            {
                minlc[i] = float.Parse(equInfos[i].ElectricalRangeEnd.ToString());//电气量程终
                maxlc[i] = float.Parse(equInfos[i].ElectricalRangeBegin.ToString());//电气量程始
                lc[i] = maxlc[i] - minlc[i];
                gp[i] = equInfos[i].ModuleChannelIn;//输入模块通道号
            }

            //读取端口号和波特率信息集合
            comSettingInfos = db.BssComSettings.Where(p => p.Deleted == false)
                .Select(p => new BssComSettingModel
                {
                    ComTarget = p.ComTarget,
                    ComNumber = p.ComNumber,
                    BaudRate = p.BaudRate,
                    CheckDigit = p.CheckDigit,
                    DataBit = p.DataBit,
                    StopBit = p.StopBit,
                    ComType = p.ComType
                }).ToList();
        }

        /// <summary>
        /// 显示默认初始数据(从基础因子表中获取)
        /// </summary>
        /// <param name="db"></param>
        private void ShowDefaultFactorData(DatabaseContext db, ref List<BssFactorModel> result)
        {
            //加载基础数据
            result = (from a in db.BasFactors.Where(p => p.Deleted == false)
                      join b in db.BasFactorEquSettings.Where(p => p.Deleted == false)
                      on a.FactorName equals b.FactorName
                      select new BssFactorModel
                      {
                          Id = b.Id,
                          FactorName = b.FactorName,
                          FactorValue = "0.00",
                          FactorUnit = b.FactorUnit,
                          FactorState = "连接中",
                          IfShow = a.IfShow == true ? Visibility.Visible : Visibility.Collapsed,
                          Sort = b.Sort
                      }).OrderBy(p => p.Sort).ToList();

            if (flueGasHumidityChecked)
            {
                result.Where(p => p.Id == 15).FirstOrDefault().FactorValue = flueGasHumidityValue.ToString("#0.00");
            }

            for (int i = 0; i < 100; i++)
            {
                factorsStates[i] = "连接中";
            }
        }
        #endregion

        #region 计时器
        private void TimerLoad()
        {
            //任务定时调度
            //Task.Run(async () =>
            //{
            //    await QuartzLoadInfo();

            //});
            // 对象初始化，以及指定相应的委托程序--每秒读取因子数据任务
            LogHelper.Info($"开启定时器，每1秒读取因子数据任务");
            loadFactorInfoTimer.Tick += new EventHandler(OnDispatcherTimer_LoadFactorInfo);
            loadFactorInfoTimer.Interval = new TimeSpan(0, 0, 0, 1);
            loadFactorInfoTimer.Start();

            // 对象初始化，以及指定相应的委托程序--每5秒读取读取modbus、PLC数据，发送212数据
            LogHelper.Info($"开启定时器，每5秒读取读取modbus、PLC数据，发送212数据任务");
            handleModbusPlc212Timer.Tick += new EventHandler(OnDispatcherTimer_HandleModbusPlc212);
            handleModbusPlc212Timer.Interval = new TimeSpan(0, 0, 0, 6);
            handleModbusPlc212Timer.Start();

            // 对象初始化，以及指定相应的委托程序--每60秒更新数据
            LogHelper.Info($"开启定时器，每60秒更新数据任务");
            updateDataTimer.Tick += new EventHandler(OnDispatcherTimer_UpdateData);
            updateDataTimer.Interval = new TimeSpan(0, 0, 0, 60);
            updateDataTimer.Start();
        }

        /// <summary>
        /// 定时5秒
        /// 1.读取Modbus
        /// 2.读取PLC
        /// 3.发送212
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnDispatcherTimer_HandleModbusPlc212(object sender, EventArgs e)
        {
            lock (timerLock)
            {
                LogHelper.Info($"开启线程，每5秒读取读取modbus、PLC数据，发送212数据任务");
                Thread thread = new Thread(HandleModbusPlc212TimerDo);  //第一步  创建线程对象  并把要交给线程执行的函数  通过参数传递给线程
                thread.IsBackground = true;             //第二步  配置线程
                thread.Start();
                LogHelper.Info($"开启线程每5秒读取读取modbus、PLC数据，发送212数据任务 成功");

                //LogHelper.Info($"开始，每5秒读取读取modbus、PLC数据，发送212数据任务");
                //HandleModbusPlc212TimerDo();
            }
        }

        /// <summary>
        /// 定时器 每1秒 刷新因子的数据和状态 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnDispatcherTimer_LoadFactorInfo(object sender, EventArgs e)
        {
            lock (timerLoadFactorLock)
            {
                try
                {
                    if (factorsStates[3] == "正在维护")
                    {
                        factorsStates[0] = "正在维护";
                    }
                    var newHomePageProjectList = HomePageProjectList;
                    HomePageProjectList = new List<BssFactorModel>();

                    #region 数值赋值
                    #region 固定写法
                    //newHomePageProjectList.Where(p => p.FactorName == "总烃").FirstOrDefault().FactorValue = value[0];//总烃值1
                    //newHomePageProjectList.Where(p => p.FactorName == "非甲烷总烃").FirstOrDefault().FactorValue = value[1];//非甲烷总烃值2
                    //newHomePageProjectList.Where(p => p.FactorName == "甲烷").FirstOrDefault().FactorValue = value[2];//甲烷值3
                    //newHomePageProjectList.Where(p => p.FactorName == "烟气温度").FirstOrDefault().FactorValue = value[3];//烟气温度值4
                    //newHomePageProjectList.Where(p => p.FactorName == "烟气压力").FirstOrDefault().FactorValue = value[4];//烟气压力值5
                    //newHomePageProjectList.Where(p => p.FactorName == "烟气流速").FirstOrDefault().FactorValue = value[5];//烟气流速值6
                    //newHomePageProjectList.Where(p => p.FactorName == "工况流量").FirstOrDefault().FactorValue = value[6];//工况流量值7
                    //newHomePageProjectList.Where(p => p.FactorName == "标况流量").FirstOrDefault().FactorValue = value[7];//标况流量值8
                    //newHomePageProjectList.Where(p => p.FactorName == "苯系物含量").FirstOrDefault().FactorValue = value[8];//苯系物值9

                    //newHomePageProjectList.Where(p => p.FactorName == "折算非甲烷总烃").FirstOrDefault().FactorValue = value[9];//非甲烷总烃折算值19
                    //newHomePageProjectList.Where(p => p.FactorName == "非甲烷总烃排放量").FirstOrDefault().FactorValue = value[10];//非甲烷总烃排放值20

                    //newHomePageProjectList.Where(p => p.FactorName == "烟尘湿值").FirstOrDefault().FactorValue = value[11];//烟尘湿值值10
                    //newHomePageProjectList.Where(p => p.FactorName == "烟尘排放量").FirstOrDefault().FactorValue = value[12];//烟尘湿值排放值11
                    //newHomePageProjectList.Where(p => p.FactorName == "氧气含量").FirstOrDefault().FactorValue = value[13];//氧气含量值12
                    //newHomePageProjectList.Where(p => p.FactorName == "烟气湿度").FirstOrDefault().FactorValue = value[14];//烟尘湿度值13
                    //newHomePageProjectList.Where(p => p.FactorName == "硫化氢").FirstOrDefault().FactorValue = value[15];//硫化氢值14
                    //newHomePageProjectList.Where(p => p.FactorName == "苯含量").FirstOrDefault().FactorValue = value[16];//苯含量15
                    //newHomePageProjectList.Where(p => p.FactorName == "甲苯").FirstOrDefault().FactorValue = value[17];//甲苯16
                    //newHomePageProjectList.Where(p => p.FactorName == "间二甲苯").FirstOrDefault().FactorValue = value[18];//间二甲苯17
                    //newHomePageProjectList.Where(p => p.FactorName == "邻二甲苯").FirstOrDefault().FactorValue = value[19];//邻二甲苯18  
                    #endregion

                    //用此方式，ID对应的T_Bas_Factor_EquSetting表中因子不要改 基础数据基本不会变动
                    //Random rd = new Random();
                    //LogHelper.Info($"给每个因子数值赋值");
                    newHomePageProjectList.ForEach(p =>
                    {
                        //烟气湿度(ID为15)被选中后，烟气湿度的值则固定为设定值
                        if (p.Id == 15 && flueGasHumidityChecked)
                        {
                            p.FactorValue = flueGasHumidityValue.ToString("#0.00");
                        }
                        else
                        {
                            p.FactorValue = (string.IsNullOrEmpty(factorsValues[p.Id - 1]) || factorsValues[p.Id - 1] == "非数字") ? "0.00" : factorsValues[p.Id - 1];
                            //p.FactorValue = value[p.Id - 1] ?? "0.00";
                        }

                        //p.FactorValue = rd.Next().ToString();//测试动态
                    });
                    #endregion

                    //bw.RunWorkerAsync();

                    #region 状态赋值
                    #region 固定写法
                    //newHomePageProjectList.Where(p => p.FactorName == "总烃").FirstOrDefault().FactorState = state[0];//总烃状态1
                    //newHomePageProjectList.Where(p => p.FactorName == "非甲烷总烃").FirstOrDefault().FactorState = state[0];//非甲烷总烃状态2
                    //newHomePageProjectList.Where(p => p.FactorName == "甲烷").FirstOrDefault().FactorState = state[0];//甲烷状态3

                    //newHomePageProjectList.Where(p => p.FactorName == "苯含量").FirstOrDefault().FactorState = state[0];//苯含量17
                    //newHomePageProjectList.Where(p => p.FactorName == "甲苯").FirstOrDefault().FactorState = state[0];//甲苯18

                    //newHomePageProjectList.Where(p => p.FactorName == "间二甲苯").FirstOrDefault().FactorState = state[0];//间二甲苯19
                    //newHomePageProjectList.Where(p => p.FactorName == "邻二甲苯").FirstOrDefault().FactorState = state[0];//邻二甲苯20

                    //newHomePageProjectList.Where(p => p.FactorName == "苯系物含量").FirstOrDefault().FactorState = state[0];//苯系物状态9

                    //newHomePageProjectList.Where(p => p.FactorName == "折算非甲烷总烃").FirstOrDefault().FactorState = state[0];//非甲烷总烃折算状态10
                    //newHomePageProjectList.Where(p => p.FactorName == "非甲烷总烃排放量").FirstOrDefault().FactorState = state[0];//非甲烷总烃排放状态11
                    #endregion
                    //LogHelper.Info($"给每个因子状态赋值");
                    var state0Array = new int[] { 1, 2, 3, 9, 10, 11, 17, 18, 19, 20, 21 };//将值赋给state0的ID集合
                    newHomePageProjectList.Where(p => state0Array.Contains(p.Id)).ToList().ForEach(p =>
                    {
                        p.FactorState = factorsStates[0];
                        //p.FactorState = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss");//测试动态
                    });

                    #region 固定写法
                    //newHomePageProjectList.Where(p => p.FactorName == "烟气温度").FirstOrDefault().FactorState = state[3];//烟气温度状态4
                    //newHomePageProjectList.Where(p => p.FactorName == "烟气压力").FirstOrDefault().FactorState = state[3];//烟气压力状态5
                    //newHomePageProjectList.Where(p => p.FactorName == "烟气流速").FirstOrDefault().FactorState = state[3];//烟气流速状态6
                    //newHomePageProjectList.Where(p => p.FactorName == "工况流量").FirstOrDefault().FactorState = state[3];//工况流量状态7
                    //newHomePageProjectList.Where(p => p.FactorName == "标况流量").FirstOrDefault().FactorState = state[3];//标况流量状态8

                    //newHomePageProjectList.Where(p => p.FactorName == "烟尘湿值").FirstOrDefault().FactorState = state[3];//烟尘湿值状态12
                    //newHomePageProjectList.Where(p => p.FactorName == "烟尘排放量").FirstOrDefault().FactorState = state[3];//烟尘湿值排放状态13
                    //newHomePageProjectList.Where(p => p.FactorName == "氧气含量").FirstOrDefault().FactorState = state[3];//氧气含量状态14
                    //newHomePageProjectList.Where(p => p.FactorName == "烟气湿度").FirstOrDefault().FactorState = state[3];//烟尘湿度状态15
                    //newHomePageProjectList.Where(p => p.FactorName == "硫化氢").FirstOrDefault().FactorState = state[3];//硫化氢状态16
                    #endregion
                    var state3Array = new int[] { 4, 5, 6, 7, 8, 12, 13, 14, 15, 16 };//将值赋给state3的ID集合
                    newHomePageProjectList.Where(p => state3Array.Contains(p.Id)).ToList().ForEach(p =>
                    {
                        p.FactorState = factorsStates[3];
                        //p.FactorState = DateTime.Now.AddDays(1).ToString("yyyy-MM-dd HH:mm:ss");//测试动态
                    });
                    #endregion

                    HomePageProjectList = newHomePageProjectList;

                    testNumber++;

                    //关闭所有的timer
                    if (GlobalVariables.IfStopTimer)
                    {
                        loadFactorInfoTimer.Stop();
                        handleModbusPlc212Timer.Stop();
                        updateDataTimer.Stop();
                    }
                }
                catch (Exception ex)
                {
                    LogHelper.Error($"给因子赋值失败，错误信息{ex}");
                    LogHelper.Info($"重启因子赋值定时器。");
                    loadFactorInfoTimer.Stop();
                    Delay(10000);
                    loadFactorInfoTimer.Start();
                    LogHelper.Info($"重启因子赋值定时器成功！");
                }
            }
        }

        /// <summary>
        /// 定时每60秒更新数据
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnDispatcherTimer_UpdateData(object sender, EventArgs e)
        {
            lock (timerUpdateLock)
            {
                try
                {
                    using (var db = new DatabaseContext())
                    {
                        var thisMinute = DateTime.Now.ToString("yyyy-MM-dd HH:mm");
                        db.HistoryMinutes.RemoveRange(db.HistoryMinutes.Where(p => p.HistoryTime == thisMinute));

                        //保存分钟历史数据
                        LogHelper.Info($"保存分钟历史");
                        var addHistoryMinute = new HistoryMinute
                        {
                            THC = factorsValues[0] ?? "0.00",//1
                            NMHC = factorsValues[1] ?? "0.00",//2
                            Methane = factorsValues[2] ?? "0.00",//3
                            FlueGasTemp = factorsValues[3] ?? "0.00",//4
                            FlueGasPress = factorsValues[4] ?? "0.00",//5
                            FlueGasVelocity = factorsValues[5] ?? "0.00",//6
                            WorkingFlow = factorsValues[6] ?? "0.00",//7
                            StandardFlow = factorsValues[7] ?? "0.00",//8
                            BenSeriesContent = factorsValues[8] ?? "0.00",//9
                            NMHCConverted = factorsValues[9] ?? "0.00",//10
                            NMHCEmiss = factorsValues[10] ?? "0.00",//11
                            DustConcentration = factorsValues[11] ?? "0.00",//12
                            DustDryValue = factorsValues[20] ?? "0.00",//21 新增的烟气干值
                            DustConcentrationEmiss = factorsValues[12] ?? "0.00",//13
                            OxygenContent = factorsValues[13] ?? "0.00",//14
                            FlueGasHumidity = factorsValues[14] ?? "0.00",//15
                            HydrogenSulfide = factorsValues[15] ?? "0.00",//16
                            BenContent = factorsValues[16] ?? "0.00",//17
                            Toluene = factorsValues[17] ?? "0.00",//18
                            MXylene = factorsValues[18] ?? "0.00",//19
                            OXylene = factorsValues[19] ?? "0.00",//20
                            Creator = GlobalVariables.UserName
                        };

                        db.HistoryMinutes.Add(addHistoryMinute);

                        //保存小时均值历史数据
                        if (DateTime.Now.Minute == 0)
                        {
                            LogHelper.Info($"保存小时历史");
                            //获取上小时中全部的数据
                            var beforeHour = DateTime.Now.AddHours(-1).ToString("yyyy-MM-dd HH");
                            var beforeHourHistories = db.HistoryMinutes.Where(p => p.Deleted == false
                            && p.HistoryTime.Contains(beforeHour)).ToList();
                            var thisHour = DateTime.Now.ToString("yyyy-MM-dd HH");

                            db.HistoryHours.RemoveRange(db.HistoryHours.Where(p => p.HistoryTime == thisHour));

                            if (beforeHourHistories != null && beforeHourHistories.Count() > 0)
                            {
                                var addHistoryHour = new HistoryHour
                                {
                                    THC = beforeHourHistories.Average(p => decimal.Parse(p.THC ?? "0.00")).ToString("#0.00"),//1
                                    NMHC = beforeHourHistories.Average(p => decimal.Parse(p.NMHC ?? "0.00")).ToString("#0.00"),//2
                                    Methane = beforeHourHistories.Average(p => decimal.Parse(p.Methane ?? "0.00")).ToString("#0.00"),//3
                                    FlueGasTemp = beforeHourHistories.Average(p => decimal.Parse(p.FlueGasTemp ?? "0.00")).ToString("#0.00"),//4
                                    FlueGasPress = beforeHourHistories.Average(p => decimal.Parse(p.FlueGasPress ?? "0.00")).ToString("#0.00"),//5
                                    FlueGasVelocity = beforeHourHistories.Average(p => decimal.Parse(p.FlueGasVelocity ?? "0.00")).ToString("#0.00"),//6
                                    WorkingFlow = beforeHourHistories.Average(p => decimal.Parse(p.WorkingFlow ?? "0.00")).ToString("#0.00"),//7
                                    StandardFlow = beforeHourHistories.Average(p => decimal.Parse(p.StandardFlow ?? "0.00")).ToString("#0.00"),//8
                                    BenSeriesContent = beforeHourHistories.Average(p => decimal.Parse(p.BenSeriesContent ?? "0.00")).ToString("#0.00"),//9
                                    NMHCConverted = beforeHourHistories.Average(p => decimal.Parse(p.NMHCConverted ?? "0.00")).ToString("#0.00"),//10
                                    NMHCEmiss = beforeHourHistories.Average(p => decimal.Parse(p.NMHCEmiss ?? "0.00")).ToString("#0.00"),//11
                                    DustConcentration = beforeHourHistories.Average(p => decimal.Parse(p.DustConcentration ?? "0.00")).ToString("#0.00"),//12
                                    DustDryValue = beforeHourHistories.Average(p => decimal.Parse(p.DustDryValue ?? "0.00")).ToString("#0.00"),//21
                                    DustConcentrationEmiss = beforeHourHistories.Average(p => decimal.Parse(p.DustConcentrationEmiss ?? "0.00")).ToString("#0.00"),//13
                                    OxygenContent = beforeHourHistories.Average(p => decimal.Parse(p.OxygenContent ?? "0.00")).ToString("#0.00"),//14
                                    FlueGasHumidity = beforeHourHistories.Average(p => decimal.Parse(p.FlueGasHumidity ?? "0.00")).ToString("#0.00"),//15
                                    HydrogenSulfide = beforeHourHistories.Average(p => decimal.Parse(p.HydrogenSulfide ?? "0.00")).ToString("#0.00"),//16
                                    BenContent = beforeHourHistories.Average(p => decimal.Parse(p.BenContent ?? "0.00")).ToString("#0.00"),//17
                                    Toluene = beforeHourHistories.Average(p => decimal.Parse(p.Toluene ?? "0.00")).ToString("#0.00"),//18
                                    MXylene = beforeHourHistories.Average(p => decimal.Parse(p.MXylene ?? "0.00")).ToString("#0.00"),//19
                                    OXylene = beforeHourHistories.Average(p => decimal.Parse(p.OXylene ?? "0.00")).ToString("#0.00"),//20
                                    Creator = GlobalVariables.UserName
                                };

                                db.HistoryHours.Add(addHistoryHour);
                            }

                            /*每个小时更新一次月均值数据*/
                            LogHelper.Info($"保存、更新月均值历史");
                            var thisMonthTime = DateTime.Now.ToString("yyyy-MM");
                            var thisMonthDays = db.HistoryDays.Where(p => p.Deleted == false && p.HistoryTime.Contains(thisMonthTime)).ToList();

                            db.HistoryMonths.RemoveRange(db.HistoryMonths.Where(p => p.Deleted == false && p.HistoryTime == thisMonthTime));

                            if (thisMonthDays != null && thisMonthDays.Count() > 0)
                            {
                                var addHistoryMonth = new HistoryMonth
                                {
                                    THC = thisMonthDays.Average(p => decimal.Parse(p.THC)).ToString("#0.00"),//1
                                    NMHC = thisMonthDays.Average(p => decimal.Parse(p.NMHC)).ToString("#0.00"),//2
                                    Methane = thisMonthDays.Average(p => decimal.Parse(p.Methane)).ToString("#0.00"),//3
                                    FlueGasTemp = thisMonthDays.Average(p => decimal.Parse(p.FlueGasTemp)).ToString("#0.00"),//4
                                    FlueGasPress = thisMonthDays.Average(p => decimal.Parse(p.FlueGasPress)).ToString("#0.00"),//5
                                    FlueGasVelocity = thisMonthDays.Average(p => decimal.Parse(p.FlueGasVelocity)).ToString("#0.00"),//6
                                    WorkingFlow = thisMonthDays.Average(p => decimal.Parse(p.WorkingFlow)).ToString("#0.00"),//7
                                    StandardFlow = thisMonthDays.Average(p => decimal.Parse(p.StandardFlow)).ToString("#0.00"),//8
                                    BenSeriesContent = thisMonthDays.Average(p => decimal.Parse(p.BenSeriesContent)).ToString("#0.00"),//9
                                    NMHCConverted = thisMonthDays.Average(p => decimal.Parse(p.NMHCConverted)).ToString("#0.00"),//10
                                    NMHCEmiss = thisMonthDays.Average(p => decimal.Parse(p.NMHCEmiss)).ToString("#0.00"),//11
                                    DustConcentration = thisMonthDays.Average(p => decimal.Parse(p.DustConcentration)).ToString("#0.00"),//12
                                    DustDryValue = thisMonthDays.Average(p => decimal.Parse(p.DustDryValue)).ToString("#0.00"),//21
                                    DustConcentrationEmiss = thisMonthDays.Average(p => decimal.Parse(p.DustConcentrationEmiss)).ToString("#0.00"),//13
                                    OxygenContent = thisMonthDays.Average(p => decimal.Parse(p.OxygenContent)).ToString("#0.00"),//14
                                    FlueGasHumidity = thisMonthDays.Average(p => decimal.Parse(p.FlueGasHumidity)).ToString("#0.00"),//15
                                    HydrogenSulfide = thisMonthDays.Average(p => decimal.Parse(p.HydrogenSulfide)).ToString("#0.00"),//16
                                    BenContent = thisMonthDays.Average(p => decimal.Parse(p.BenContent)).ToString("#0.00"),//17
                                    Toluene = thisMonthDays.Average(p => decimal.Parse(p.Toluene)).ToString("#0.00"),//18
                                    MXylene = thisMonthDays.Average(p => decimal.Parse(p.MXylene)).ToString("#0.00"),//19
                                    OXylene = thisMonthDays.Average(p => decimal.Parse(p.OXylene)).ToString("#0.00"),//20
                                    Creator = GlobalVariables.UserName
                                };

                                db.HistoryMonths.Add(addHistoryMonth);
                            }
                        }

                        //保存日均值历史数据
                        if (DateTime.Now.Hour == 0 && DateTime.Now.Minute == 0)
                        {
                            LogHelper.Info($"保存日均值历史");
                            var beforeDayTime = DateTime.Now.AddDays(-1).ToString("yyyy-MM-dd");
                            var beforeDayHourInfos = db.HistoryHours.Where(p => p.Deleted == false && p.HistoryTime.Contains(beforeDayTime)).ToList();

                            var thisDayTime = DateTime.Now.ToString("yyyy-MM-dd");

                            db.HistoryDays.RemoveRange(db.HistoryDays.Where(p => p.HistoryTime == thisDayTime));

                            if (beforeDayHourInfos != null && beforeDayHourInfos.Count() > 0)
                            {
                                var addHistoryDay = new HistoryDay
                                {
                                    THC = beforeDayHourInfos.Average(p => decimal.Parse(p.THC)).ToString("#0.00"),//1
                                    NMHC = beforeDayHourInfos.Average(p => decimal.Parse(p.NMHC)).ToString("#0.00"),//2
                                    Methane = beforeDayHourInfos.Average(p => decimal.Parse(p.Methane)).ToString("#0.00"),//3
                                    FlueGasTemp = beforeDayHourInfos.Average(p => decimal.Parse(p.FlueGasTemp)).ToString("#0.00"),//4
                                    FlueGasPress = beforeDayHourInfos.Average(p => decimal.Parse(p.FlueGasPress)).ToString("#0.00"),//5
                                    FlueGasVelocity = beforeDayHourInfos.Average(p => decimal.Parse(p.FlueGasVelocity)).ToString("#0.00"),//6
                                    WorkingFlow = beforeDayHourInfos.Average(p => decimal.Parse(p.WorkingFlow)).ToString("#0.00"),//7
                                    StandardFlow = beforeDayHourInfos.Average(p => decimal.Parse(p.StandardFlow)).ToString("#0.00"),//8
                                    BenSeriesContent = beforeDayHourInfos.Average(p => decimal.Parse(p.BenSeriesContent)).ToString("#0.00"),//9
                                    NMHCConverted = beforeDayHourInfos.Average(p => decimal.Parse(p.NMHCConverted)).ToString("#0.00"),//10
                                    NMHCEmiss = beforeDayHourInfos.Average(p => decimal.Parse(p.NMHCEmiss)).ToString("#0.00"),//11
                                    DustConcentration = beforeDayHourInfos.Average(p => decimal.Parse(p.DustConcentration)).ToString("#0.00"),//12
                                    DustDryValue = beforeDayHourInfos.Average(p => decimal.Parse(p.DustDryValue)).ToString("#0.00"),//21
                                    DustConcentrationEmiss = beforeDayHourInfos.Average(p => decimal.Parse(p.DustConcentrationEmiss)).ToString("#0.00"),//13
                                    OxygenContent = beforeDayHourInfos.Average(p => decimal.Parse(p.OxygenContent)).ToString("#0.00"),//14
                                    FlueGasHumidity = beforeDayHourInfos.Average(p => decimal.Parse(p.FlueGasHumidity)).ToString("#0.00"),//15
                                    HydrogenSulfide = beforeDayHourInfos.Average(p => decimal.Parse(p.HydrogenSulfide)).ToString("#0.00"),//16
                                    BenContent = beforeDayHourInfos.Average(p => decimal.Parse(p.BenContent)).ToString("#0.00"),//17
                                    Toluene = beforeDayHourInfos.Average(p => decimal.Parse(p.Toluene)).ToString("#0.00"),//18
                                    MXylene = beforeDayHourInfos.Average(p => decimal.Parse(p.MXylene)).ToString("#0.00"),//19
                                    OXylene = beforeDayHourInfos.Average(p => decimal.Parse(p.OXylene)).ToString("#0.00"),//20
                                    Creator = GlobalVariables.UserName
                                };

                                db.HistoryDays.Add(addHistoryDay);
                            }
                        }

                        var result = db.SaveChanges() > 0;
                        //var msg = result ? "成功" : "失败";
                        //MessageBox.Show($"保存{msg}!");
                    }
                }
                catch (Exception ex)
                {
                    LogHelper.Error($"保存历史数据出错", ex);
                    LogHelper.Info($"重启保存历史数据计时器。");
                    updateDataTimer.Stop();
                    Delay(60000);
                    updateDataTimer.Start();
                    LogHelper.Info($"重启保存历史数据计时器成功！");
                }


                //if (GlobalVariables.UserName != "超级管理员")//超级管理员不校验
                //{
                //    DateTime now = DateTime.Now;
                //    try
                //    {
                //        //试用时间已到期，打开输入激活码页面
                //        if (now > GlobalVariables.Validity)
                //        {
                //            LogHelper.Warn($"试用期到期");
                //            ActivateSystem activateSystemWin = new ActivateSystem();
                //            activateSystemWin.ShowDialog();
                //        }
                //    }
                //    catch (Exception ex)
                //    {
                //        LogHelper.Error($"校验试用期出错", ex);
                //    }
                //}
            }
        }
        #endregion

        #region 数据读取
        //读取色谱仪 PLC 
        //发送HJ/212
        public void HandleModbusPlc212TimerDo()
        {
            LogHelper.Info($"开始读取色谱仪、PLC、发送HJ/212。");
            try
            {
                flag212_1 = "N";//每次默认是正常
                flag212_2 = "N";

                //从ComSetting表中循环初始化各个接口
                LoadComsFromSetting();

                //读取PLC或模块数据
                switch (PLCType)
                {
                    case "VOCS":
                        LogHelper.Info($"PLCType--VOCS");
                        ReadTAP(gp, lc, minlc, maxlc);//PLC读取温压流数据
                        break;
                    case "VOC":
                        LogHelper.Info($"PLCType--VOC");
                        ReadVOC();
                        break;
                }

                string EJB = "0.00";//二甲苯=间二甲苯+邻二甲苯
                if (caliFlag)//是否在校准模式 校准模式时不读取色谱仪
                {
                    //根据色谱协议读取数据
                    string zongting = "", jiawan = "", feijiawanzongting = "";//这是临时的数据存储空间
                    string ben = "", jiaben = "", jianerjiaben = "", lingerjiaben = "", yiben = "", benyixi = "";
                    switch (vocType)
                    {
                        case "SPRT"://斯普瑞特
                            zongting = toolHelper.Total_Hydrocarbon(1, this.spModbus, 3);//总烃值
                            jiawan = toolHelper.Total_Hydrocarbon(2, this.spModbus, 3);//甲烷值
                            ben = toolHelper.Total_Hydrocarbon(gp[16], this.spModbus, 3);//读取苯含量
                            jiaben = toolHelper.Total_Hydrocarbon(gp[17], this.spModbus, 3);//读取甲苯
                            jianerjiaben = toolHelper.Total_Hydrocarbon(gp[18], this.spModbus, 3);//读取间二甲苯
                            lingerjiaben = toolHelper.Total_Hydrocarbon(gp[19], this.spModbus, 3);//读取邻二甲苯
                            break;
                        case "鲁南"://山东鲁南
                            zongting = toolHelper.Total_Hydrocarbon_NBGC60(1, this.spModbus, 3);//总烃值
                            jiawan = toolHelper.Total_Hydrocarbon_NBGC60(2, this.spModbus, 3);//甲烷值
                            ben = toolHelper.Total_Hydrocarbon_NBGC60(4, this.spModbus, 3);//读取苯
                            jiaben = toolHelper.Total_Hydrocarbon_NBGC60(5, this.spModbus, 3);//读取甲苯
                            jianerjiaben = toolHelper.Total_Hydrocarbon_NBGC60(6, this.spModbus, 3);//读取间二甲苯
                            lingerjiaben = toolHelper.Total_Hydrocarbon_NBGC60(7, this.spModbus, 3);//读取邻二甲苯
                            break;
                        case "天蓝"://天蓝
                            byte[] needGetAddresses = new byte[] { 0x14, 0x16 };
                            var tianLanResult = toolHelper.Total_Hydrocarbon_TianLan(needGetAddresses, this.spModbus, 3);//同时获取总烃和甲烷数据
                            zongting = tianLanResult[0];//总烃值
                            jiawan = tianLanResult[1];//甲烷值

                            //苯系物暂时全部赋值0 20221017
                            ben = "0.00";//读取苯
                            jiaben = "0.00";//读取甲苯
                            jianerjiaben = "0.00";//读取间二甲苯
                            lingerjiaben = "0.00";//读取邻二甲苯
                            break;
                    }

                    LogHelper.Info($"VOC类型-{vocType}，读取数据结果：总烃-{zongting};甲烷-{jiawan};苯-{ben};" +
                        $"甲苯-{jiaben};间二甲苯-{jianerjiaben};邻二甲苯-{lingerjiaben}。");

                    //色谱仪的值和状态都是独立读取的，此处统一处理
                    if (zongting == "连接超时" || zongting == "非数字" || jiawan == "连接超时" || jiawan == "非数字")
                    {
                        factorsStates[0] = "连接超时";
                        factorsStates[1] = "连接超时";
                        factorsStates[2] = "连接超时";
                        factorsValues[0] = "0.00";
                        factorsValues[1] = "0.00";
                        factorsValues[2] = "0.00";
                        factorsValues[9] = "0.00";
                        factorsValues[10] = "0.00";
                        flag212_1 = "D";//故障

                        GlobalVariables.MaxErrorTimes++;
                        LogHelper.Info($"连接超时次数：{GlobalVariables.MaxErrorTimes}");
                    }
                    else
                    {
                        factorsStates[0] = "测量正常";
                        factorsStates[1] = "测量正常";
                        factorsStates[2] = "测量正常";
                        feijiawanzongting = (float.Parse(zongting) - float.Parse(jiawan)).ToString("#0.00");//非甲烷总统值
                        factorsValues[0] = zongting;
                        factorsValues[1] = feijiawanzongting;//非甲烷总烃
                        factorsValues[2] = jiawan;//甲烷赋值
                        factorsValues[9] = feijiawanzongting;//折算非甲烷总烃
                        factorsValues[10] = (float.Parse(feijiawanzongting) * float.Parse(factorsValues[7]) / 1000000).ToString("0.0000");//非甲烷总烃排放量=非甲烷总烃值* 标况流量*10-6  单位 kg/h
                        if (float.Parse(factorsValues[1]) < 0) factorsValues[1] = "0.00";//如果非甲烷总烃出现负值,改为0
                        if (float.Parse(factorsValues[9]) < 0) factorsValues[9] = "0.00";
                        if (float.Parse(factorsValues[10]) < 0) factorsValues[10] = "0.00";
                    }

                    if (ben == "连接超时" || jiaben == "连接超时" || jianerjiaben == "连接超时" || lingerjiaben == "连接超时")
                    {
                        factorsStates[16] = "连接超时";
                        factorsStates[17] = "连接超时";
                        factorsStates[18] = "连接超时";
                        factorsStates[19] = "连接超时";
                        factorsValues[16] = "0.00";
                        factorsValues[17] = "0.00";
                        factorsValues[18] = "0.00";
                        factorsValues[19] = "0.00";
                        flag212_1 = "D";//故障
                    }
                    else
                    {
                        factorsStates[6] = "测量正常";
                        factorsStates[16] = "测量正常";
                        factorsStates[17] = "测量正常";
                        factorsStates[18] = "测量正常";
                        factorsStates[19] = "测量正常";

                        if (ben != "非数字" && float.Parse(ben) >= 0 && float.Parse(ben) < 1000)
                        {
                            factorsValues[16] = ben;
                        }
                        if (jiaben != "非数字" && float.Parse(jiaben) >= 0 && float.Parse(jiaben) < 1000)
                        {
                            factorsValues[17] = jiaben;
                        }
                        if (jianerjiaben != "非数字" && float.Parse(jianerjiaben) >= 0 && float.Parse(jianerjiaben) < 1000)
                        {
                            factorsValues[18] = jianerjiaben;
                        }
                        if (lingerjiaben != "非数字" && float.Parse(lingerjiaben) >= 0 && float.Parse(lingerjiaben) < 1000)
                        {
                            factorsValues[19] = lingerjiaben;
                        }
                        float benSum = 0;//苯系物总和
                        benSum = float.Parse(ben) + float.Parse(jiaben) + float.Parse(jianerjiaben) + float.Parse(lingerjiaben);
                        factorsValues[8] = benSum.ToString("#0.00");
                    }
                }
                else
                {
                    flag212_1 = "C";//校准
                    flag212_2 = "C";
                    for (int i = 0; i < 20; i++)
                    {
                        factorsStates[i] = "正在校准";
                    }
                }

                //处理212
                Handle212(HJ212Type, HJ212Type_2, EJB);

            }
            catch (Exception ex)
            {
                LogHelper.Error($"读取色谱仪、PLC出错", ex);
            }
            finally
            {
                //释放接口资源
                LogHelper.Info($"开始释放各个接口资源。");
                this.spPLC.Dispose();
                this.spModbus.Dispose();
                this.spHJ212.Dispose();
                this.spHJ212_2.Dispose();
                LogHelper.Info($"释放各个接口资源成功！");

                if (GlobalVariables.MaxErrorTimes >= 10)
                {
                    LogHelper.Info($"连接超时{GlobalVariables.MaxErrorTimes}次，开始重启计时器。");
                    GlobalVariables.MaxErrorTimes = 0;

                    LogHelper.Info($"重启数据读取定时器。");
                    handleModbusPlc212Timer.Stop();
                    Delay(10000);
                    handleModbusPlc212Timer.Start();
                    LogHelper.Info($"重启数据读取定时器成功！");
                }
            }
        }

        /// <summary>
        /// 212协议的处理 从本机发送参数到数采仪
        /// </summary>
        /// <param name="HJ212Type"></param>
        /// <param name="HJ212Type_2"></param>
        /// <param name="EJB"></param>
        private void Handle212(string HJ212Type, string HJ212Type_2, string EJB)
        {
            try
            {
                LogHelper.Info($"212协议的处理 从本机发送参数到数采仪");

                for (int i = 0; i < 2; i++)
                {
                    SerialPort tempsp = null;//for循环中第一次为一号数采仪  第二次位二号数采仪
                    string temptype = string.Empty;
                    if (i == 0)
                    {
                        tempsp = this.spHJ212;
                        temptype = HJ212Type;
                    }
                    else
                    {
                        tempsp = this.spHJ212_2;
                        temptype = HJ212Type_2;
                    }

                    string ts = ToolsHepler.GetChinaTicks(DateTime.Now);//获取时间戳
                    string baotou = "##";//包头
                    string QN = "QN=" + ts + "000;";//时间戳
                    string ST = "ST=27;";//系统编码
                    string CN = "CN=2011;";//命令编码
                    string PW = "PW=100000;";//访问密码
                    string MS = "MN=010000A8900016F000169DC0;";//唯一标识符
                    string Flag = "Flag=5;";//拆分包

                    string CP = "CP=&&" + "DataTime=" + ts + ";";

                    LogHelper.Info($"协议-{temptype}，开始处理数据。");
                    if (temptype == "2017")
                    {//2017版本的国标协议
                        foreach (var factor in showFactorNames)
                        {
                            //Console.WriteLine(sn);
                            switch (factor)
                            {
                                case "总烃":
                                    CP += "a24087-Rtd=" + factorsValues[0] + ",";//总烃
                                    CP += "a24087-Flag=" + flag212_1 + ";";
                                    break;
                                case "非甲烷总烃":
                                    CP += "a24088-Rtd=" + factorsValues[1] + ",";//非甲烷总烃
                                    CP += "a24088-ZsRtd=" + factorsValues[9] + ",";//非甲烷折算
                                    CP += "a24088-Flag=" + flag212_1 + ";";
                                    break;
                                case "甲烷":
                                    CP += "a05002-Rtd=" + factorsValues[2] + ",";//甲烷
                                    CP += "a05002-Flag=" + flag212_1 + ";";
                                    break;
                                case "烟气温度":
                                    CP += "a01012-Rtd=" + factorsValues[3] + ",";//烟气温度
                                    CP += "a01012-Flag=" + flag212_2 + ";";
                                    break;
                                case "烟气压力":
                                    CP += "a01013-Rtd=" + factorsValues[4] + ",";//烟气压力
                                    CP += "a01013-Flag=" + flag212_2 + ";";
                                    break;
                                case "烟气流速":
                                    CP += "a01011-Rtd=" + factorsValues[5] + ",";//烟气流速
                                    CP += "a01011-Flag=" + flag212_2 + ";";
                                    break;
                                case "氧气含量":
                                    CP += "a19001-Rtd=" + factorsValues[13] + ",";//氧气含量
                                    CP += "a19001-Flag=" + flag212_2 + ";";
                                    break;
                                case "烟气湿度":
                                    CP += "a01014-Rtd=" + factorsValues[14] + ",";//烟尘湿度
                                    CP += "a01014-Flag=" + flag212_2 + ";";
                                    break;
                                case "标况流量":
                                    CP += "a00000-Rtd=" + (float.Parse(factorsValues[7]) / 3600).ToString() + ",";//标况流量
                                    CP += "a00000-Flag=" + flag212_2 + ";";
                                    break;
                                case "硫化氢":
                                    CP += "a21028-Rtd=" + factorsValues[15] + ",";
                                    CP += "a21028-Flag=" + flag212_2 + ";";
                                    break;
                                case "苯含量":
                                    CP += "a25002-Rtd=" + factorsValues[16] + ",";
                                    CP += "a25002-Flag=" + flag212_1 + ";";
                                    break;
                                case "甲苯":
                                    CP += "a25003-Rtd=" + factorsValues[17] + ",";
                                    CP += "a25003-Flag=" + flag212_1 + ";";
                                    break;
                                case "间二甲苯":
                                    //CP += "a25005-Rtd=" + EJB + ",";
                                    CP += "a25005-Rtd=" + (float.Parse(factorsValues[18]) + float.Parse(factorsValues[19])).ToString() + ",";
                                    CP += "a25005-Flag=" + flag212_1 + ";";
                                    break;
                            }
                        }
                        CP += "&&";
                    }
                    else//2005版本的国标协议
                    {
                        foreach (var sn in showFactorNames)
                        {
                            switch (sn)
                            {
                                case "烟气温度":
                                    CP += "S03-Rtd=" + factorsValues[3] + ",";//烟气温度
                                    CP += "S03-Flag=" + flag212_1 + ";";
                                    break;
                                case "烟气压力":
                                    CP += "S08-Rtd=" + (float.Parse(factorsValues[4]) * 1000).ToString("#0.00") + ",";//烟气压力
                                    CP += "S08-Flag=" + flag212_2 + ";";
                                    break;
                                case "烟气流速":
                                    CP += "S02-Rtd=" + factorsValues[5] + ",";//烟气流速
                                    CP += "S02-Flag=" + flag212_2 + ";";
                                    break;
                                case "氧气含量":
                                    CP += "S01-Rtd=" + factorsValues[13] + ",";//氧气含量
                                    CP += "S01-Flag=" + flag212_2 + ";";
                                    break;
                                case "烟气湿度":
                                    CP += "S05-Rtd=" + factorsValues[14] + ",";//烟尘湿度
                                    CP += "S05-Flag=" + flag212_2 + ";";
                                    break;
                            }
                        }
                        CP += "&&";
                    }

                    string datablock = QN + ST + CN + PW + MS + Flag + CP;//拼接除校验和包尾的部分

                    int datalen = datablock.Length;//数据段的字节长度;
                    LogHelper.Info($"协议-{temptype}，datablock数据-{datablock},字段长度-{datalen}");

                    string lenth = datalen.ToString();
                    while (lenth.Length < 4) lenth = "0" + lenth;//长度不够补0
                    uint crc = ToolsHepler.CRC16_Checkout(Encoding.Default.GetBytes(datablock), (uint)datalen);//CRC校验原始数据
                    string bufcrc = Convert.ToString(crc, 16);//需要发送的crc
                    string sned212 = baotou + lenth + datablock + bufcrc + "\r\n";//最终要发送的数据

                    LogHelper.Info($"212协议-{temptype}，需要发送的crc-{bufcrc},最终要发送的212数据-{sned212}");

                    if (hjtextshow && temptype == "2017")
                    {
                        MessageBox.Show(sned212);
                        hjtextshow = false;
                    }

                    Console.WriteLine(sned212);
                    string[] ports = SerialPort.GetPortNames();

                    if (ports != null && ports.Length > 0)
                    {
                        LogHelper.Info($"212-获取到的端口：{string.Join(",", ports.ToArray())}");
                    }

                    try
                    {
                        LogHelper.Info($"212协议，串口号：{tempsp.PortName}");
                        foreach (string item in ports)
                        {
                            if (tempsp.PortName == item)
                            {
                                if (!tempsp.IsOpen)
                                {
                                    LogHelper.Info($"212协议，串口未打开，尝试开启。");
                                    tempsp.Open();
                                    LogHelper.Info($"212协议，串口开启成功，串口号{tempsp.PortName}");
                                }
                                tempsp.Write(sned212);
                                tempsp.Close();
                                break;
                            }
                        }
                    }
                    catch (Exception ex)
                    {
                        LogHelper.Error($"212协议的处理，循环写入数据出错", ex);
                        GlobalVariables.MaxErrorTimes++;
                    }
                    finally
                    {
                        tempsp.Dispose();
                    }

                }
            }
            catch (Exception ex)
            {
                LogHelper.Error($"212协议的处理，从本机发送参数到数采仪出错", ex);
            }
        }

        /// <summary>
        /// PLC读取温压流数据
        /// </summary>
        /// <param name="gp"></param>
        /// <param name="lc"></param>
        /// <param name="minlc"></param>
        /// <param name="maxlc"></param>
        public void ReadTAP(int[] gp, float[] lc, float[] minlc, float[] maxlc)
        {
            try
            {
                LogHelper.Info($"PLC开始读取温压流数据");
                if (ifReadPLC)
                {
                    float Kv = 0;//速度场系数
                    float Vp = 0;//CMS湿排气流速
                    float Vs = 0;//断面湿排气流速
                    float F = 0;//断面面积
                    float Ba = 0;//大气压力
                    float Ps = 0;//烟气压力
                    float ts = 0;//烟气温度
                    float Xsw = 0;//湿度

                    Byte[] sendbuf = new Byte[8];//声明一个临时数组存储当前来的串口数据  (byte型 数据)//15 03 00 68 00 20 C6 DA
                    sendbuf[0] = 0x15;//地址
                    sendbuf[1] = 0x04;//读功能码
                    sendbuf[2] = 0x00;//寄存器地址高位
                    sendbuf[3] = 0x10;//寄存器地址低位
                    sendbuf[4] = 0x00;//寄存器个数高位
                    sendbuf[5] = 0x10;//寄存器个数低位
                    sendbuf[6] = 0xF3;//CRC校验高位
                    sendbuf[7] = 0x17;//CRC校验低位

                    try
                    {
                        Byte[] recebuf = new Byte[70];//创建一个存储区

                        if (!spPLC.IsOpen)//PLC串口是否打开 true打开 false关闭
                        {
                            LogHelper.Info($"PLC串口未打开，尝试打开串口");
                            spPLC.Open();//打开串口
                            LogHelper.Info($"PLC串口打开成功，串口号{spPLC.PortName}");
                        }

                        spPLC.Write(sendbuf, 0, 8);//发送指令
                        LogHelper.Info($"PLC发送指令成功");
                        Delay(200);
                        int n = spPLC.BytesToRead;//接收到的指令长度
                        LogHelper.Info($"1PLC接收到的指令长度{n}");

                        int overtime = 0;
                        //MessageBox.Show(n.ToString());
                        while (n < 30)//等待数据接收完整
                        {
                            spPLC.Write(sendbuf, 0, 8);//发送指令
                            Delay(200);
                            n = spPLC.BytesToRead;

                            if (n < 30 && overtime >= overTimes)
                            {
                                //value[3] = value[4] = value[5] = value[6] = value[7] = value[11] = value[12] = value[13] = value[14] = value[15] = "0.00";
                                factorsValues[3] = factorsValues[4] = factorsValues[5] = factorsValues[6] = factorsValues[7] = factorsValues[11] = factorsValues[20] = factorsValues[12] = factorsValues[13] = factorsValues[15] = "0.00";

                                factorsValues[14] = flueGasHumidityChecked == true ? flueGasHumidityValue.ToString("#0.00") : "0.00";//烟气湿度特殊处理

                                factorsStates[3] = "连接超时";
                                flag212_2 = "D";
                                spPLC.Close();//关闭串口
                                return;
                            }
                            overtime++;//超时计数
                        }
                        spPLC.Read(recebuf, 0, 40);//读取缓冲数据  
                        int[] parMa = new int[16];//存放读取到的PLC参数电流值 单位毫安

                        parMa[0] = recebuf[gp[3] * 2 + 1] << 8 | recebuf[gp[3] * 2 + 2];//烟气温度初始值
                        parMa[1] = recebuf[gp[4] * 2 + 1] << 8 | recebuf[gp[4] * 2 + 2];//烟气压力初始值
                        parMa[2] = recebuf[gp[5] * 2 + 1] << 8 | recebuf[gp[5] * 2 + 2];//烟气流速初始值
                        parMa[3] = recebuf[gp[11] * 2 + 1] << 8 | recebuf[gp[11] * 2 + 2];//烟尘湿值初始值
                        parMa[4] = recebuf[gp[13] * 2 + 1] << 8 | recebuf[gp[13] * 2 + 2];//氧气含量初始值
                        parMa[5] = recebuf[gp[14] * 2 + 1] << 8 | recebuf[gp[14] * 2 + 2];//烟尘湿度度初始值
                        parMa[6] = recebuf[gp[15] * 2 + 1] << 8 | recebuf[gp[15] * 2 + 2];//硫化氢初始值

                        //限制数值上下限
                        for (int i = 0; i < 6; i++)
                        {
                            if (parMa[i] > 27648) parMa[i] = 27648;
                            if (parMa[i] < 5530) parMa[i] = 5530;
                        }

                        factorsValues[3] = (((float)(parMa[0] - 5530) / 22118 * lc[3]) + minlc[3]).ToString("#0.00").ToString();//烟气温度最终值
                        factorsValues[4] = (((float)(parMa[1] - 5530) / 22118 * lc[4] + minlc[4])).ToString("#0.0000").ToString();//烟气压力最终值
                        factorsValues[5] = (((float)(parMa[2] - 5530) / 22118 * lc[5]) + minlc[5]).ToString("#0.00").ToString();//烟气流速最终值
                        factorsValues[11] = (((float)(parMa[3] - 5530) / 22118 * lc[11]) + minlc[11]).ToString("#0.00").ToString();//烟尘湿值最终值
                        factorsValues[13] = (((float)(parMa[4] - 5530) / 22118 * lc[13]) + minlc[13]).ToString("#0.00").ToString();//氧气含量最终值

                        factorsValues[14] = flueGasHumidityChecked == true ? flueGasHumidityValue.ToString("#0.00")
                            : (((float)(parMa[5] - 5530) / 22118 * lc[14]) + minlc[14]).ToString("#0.00").ToString();//烟气湿度最终值，特殊处理

                        factorsValues[15] = (((float)(parMa[6] - 5530) / 22118 * lc[15]) + minlc[15]).ToString("#0.00").ToString();//硫化氢最终值

                        //var fixParaSetting = GetFixParaSettingInfo();//读取系统设置参数

                        Kv = float.Parse(fixParaSetting.VelocityFieldValue.Value.ToString());//速度场系数
                        Vp = float.Parse(factorsValues[5]);
                        Vs = Kv * Vp;
                        F = float.Parse(fixParaSetting.FlueArea.Value.ToString());//烟道面积
                        Ba = float.Parse(fixParaSetting.AtmosphericPressure.Value.ToString());//大气压力
                        Ps = float.Parse(factorsValues[4]);
                        ts = float.Parse(factorsValues[3]);
                        Xsw = float.Parse(fixParaSetting.FlueGasHumidity.Value.ToString());//烟气湿度

                        float Qs = 3600 * F * Kv * Vp;
                        factorsValues[6] = Qs.ToString("#0.00");//工况流量
                        factorsValues[7] = (Qs * (273 / (273 + ts)) * ((Ba + Ps) / 101325) * (1 - (Xsw / 100))).ToString("#0.00");//标况流量（Qsn）=Qs*(273/(273+ts))*((Ba+Ps)/101325)*(1-Xsw/100)

                        factorsValues[20] = (float.Parse(factorsValues[11]) / (1 - Xsw / 100)).ToString("#0.00");//烟尘干值 =烟尘湿值/（1-Xsw）
                        factorsValues[12] = (float.Parse(factorsValues[20]) * float.Parse(factorsValues[7]) / 1000000).ToString("0.0000");//烟尘排放量=烟尘湿度*标况流量/1000000 修改为 烟尘干值*标况流量/1000000

                        sendbuf[0] = 0x15;//地址
                        sendbuf[1] = 0x02;//读功能码
                        sendbuf[2] = 0x00;//寄存器地址高位
                        sendbuf[3] = 0x00;//寄存器地址低位
                        sendbuf[4] = 0x00;//寄存器个数高位
                        sendbuf[5] = 0x14;//寄存器个数低位
                        sendbuf[6] = 0x7B;//CRC校验高位
                        sendbuf[7] = 0x11;//CRC校验低位

                        if (!spPLC.IsOpen)//PLC串口是否打开 true打开 false关闭
                        {
                            LogHelper.Info($"PLC开始打开串口");
                            spPLC.Open();//打开串口
                            LogHelper.Info($"PLC打开串口成功，串口号{spPLC.PortName}");
                        }

                        recebuf = new Byte[50];//创建一个存储区
                        spPLC.Write(sendbuf, 0, 8);//发送指令
                        Delay(200);
                        n = spPLC.BytesToRead;//接收到的指令长度
                        LogHelper.Info($"2PLC接收到的指令长度{n}");

                        overtime = 0;
                        while (n < 8)//等待数据接收完整
                        {
                            spPLC.Write(sendbuf, 0, 8);//发送指令
                            Delay(100);
                            n = spPLC.BytesToRead;
                            LogHelper.Info($"3PLC接收到的指令长度{n}");

                            overtime++;//超时计数
                            if (overtime >= overTimes)
                            {
                                //value[3] = value[4] = value[5] = value[6] = value[7] = value[11] = value[12] = value[13] = value[14] = value[15] = "0.00";
                                factorsValues[3] = factorsValues[4] = factorsValues[5] = factorsValues[6] = factorsValues[7] = factorsValues[11] = factorsValues[20] = factorsValues[12] = factorsValues[13] = factorsValues[15] = "0.00";

                                factorsValues[14] = flueGasHumidityChecked == true ? flueGasHumidityValue.ToString("#0.00") : "0.00";//烟气湿度特殊处理

                                factorsStates[3] = "连接超时";
                                flag212_2 = "D";
                                spPLC.Close();//关闭串口

                                return;
                            }
                        }
                        spPLC.Read(recebuf, 0, 8);//读取缓冲数据  
                        if (recebuf[3] == 0x00)
                        {
                            //维护状态
                            flag212_1 = flag212_2 = "M";
                            factorsStates[3] = "正在维护";
                        }
                        else
                        {
                            factorsStates[3] = factorsStates[4] = factorsStates[5] = factorsStates[6] = factorsStates[7] = factorsStates[11] = factorsStates[20] = factorsStates[12] = factorsStates[13] = factorsStates[14] = factorsStates[15] = "测量正常";
                        }

                    }
                    catch (Exception ex)
                    {
                        LogHelper.Error($"PLC读取温压流数据出错", ex);
                        spPLC.Close();//关闭串口
                        factorsValues[3] = factorsValues[4] = factorsValues[5] = factorsValues[6] = factorsValues[7] = factorsValues[11] = factorsValues[20] = factorsValues[12] = factorsValues[13] = factorsValues[15] = "0.00";

                        factorsValues[14] = flueGasHumidityChecked == true ? flueGasHumidityValue.ToString("#0.00") : "0.00";//烟气湿度特殊处理

                        factorsStates[3] = "连接超时";
                        flag212_2 = "D";

                        GlobalVariables.MaxErrorTimes++;
                    }

                    if (tapflag)    // //PLC读取状态标志
                    {
                        sendbuf[0] = 0x15;//地址
                        sendbuf[1] = 0x03;//读功能码
                        sendbuf[2] = 0x00;//寄存器地址高位
                        sendbuf[3] = 0x32;//寄存器地址低位
                        sendbuf[4] = 0x00;//寄存器个数高位
                        sendbuf[5] = 0x02;//寄存器个数低位
                        sendbuf[6] = 0x66;//CRC校验高位
                        sendbuf[7] = 0xD0;//CRC校验低位

                        try
                        {
                            if (!this.spPLC.IsOpen)
                            {
                                LogHelper.Info($"PLC开始打开串口");
                                this.spPLC.Open();
                                LogHelper.Info($"PLC打开串口成功，串口号{spPLC.PortName}");
                            }
                            Byte[] recebuf = new Byte[9];//创建一个存储区
                            this.spPLC.Write(sendbuf, 0, 8);//发送指令
                            Delay(200);
                            int n = this.spPLC.BytesToRead;//接收到的指令长度
                            LogHelper.Info($"4PLC接收到的指令长度{n}");

                            int overtime = 0;

                            LogHelper.Info($"PLC发送指令，开始等待数据结束完整。");
                            while (n < 9)//等待数据接收完整
                            {
                                if (!this.spPLC.IsOpen)
                                {
                                    this.spPLC.Open();
                                }
                                this.spPLC.Write(sendbuf, 0, 8);//发送指令
                                Delay(200);

                                n = this.spPLC.BytesToRead;
                                LogHelper.Info($"5PLC接收到的指令长度{n}");

                                overtime++;//超时计数
                                if (overtime >= 3)
                                {
                                    this.spPLC.Close();//关闭串口
                                }
                            }
                            this.spPLC.Read(recebuf, 0, 9);//读取缓冲数据  
                            this.spPLC.Close();//关闭串口
                            this.timeCYB = recebuf[3] << 8 | recebuf[4];    // //PLC读取采样泵和反吹时间
                            this.timeFC = (recebuf[5] << 8 | recebuf[6]) / 600;
                            tapflag = false;
                        }
                        catch (Exception ex)
                        {
                            LogHelper.Error($"PLC发送指令出错", ex);
                            this.spPLC.Close();//关闭串口

                            GlobalVariables.MaxErrorTimes++;
                        }
                    }
                }
            }
            catch (Exception ex)
            {
                LogHelper.Error($"PLC读取温压流数据出错", ex);
            }
        }

        /// <summary>
        /// 温压流模块读取
        /// </summary>
        public void ReadVOC()
        {
            LogHelper.Info($"温压流模块开始读取");

            float Kv = 0;//速度场系数
            float Vp = 0;//CMS湿排气流速
            float Vs = 0;//断面湿排气流速
            float F = 0;//断面面积
            float Ba = 0;//大气压力
            float Ps = 0;//烟气压力
            float ts = 0;//烟气温度
            float Xsw = 0;//湿度

            Byte[] sendbuf = new Byte[8];//声明一个临时数组存储当前来的串口数据从机接收到是ASCII码这里是转成了十六进制
            sendbuf[0] = 0x01;
            sendbuf[1] = 0x03;
            sendbuf[2] = 0x00;
            sendbuf[3] = 0x20;
            sendbuf[4] = 0x00;
            sendbuf[5] = 0x03;
            sendbuf[6] = 0x04;
            sendbuf[7] = 0x01;

            try
            {
                Byte[] recebuf = new Byte[11];//创建一个存储区
                if (!spPLC.IsOpen)//PLC串口是否打开 true打开 false关闭
                {
                    LogHelper.Info($"温压流模块PLC串口未打开，尝试打开");
                    spPLC.Open();//打开串口
                    LogHelper.Info($"温压流模块PLC串口打开，串口号{spPLC.PortName}");
                }

                spPLC.Write(sendbuf, 0, 8);//发送指令
                LogHelper.Info($"温压流模块PLC串口发送指令成功");

                Delay(100);
                int n = spPLC.BytesToRead;//接收到的指令长度
                LogHelper.Info($"温压流模块PLC串口接收指令长度{n}");

                int overtime = 0;//超时计数
                while (n < 11)//等待数据接收完整
                {
                    spPLC.Write(sendbuf, 0, 8);//发送指令
                    LogHelper.Info($"温压流模块PLC串口发送指令成功");

                    Delay(100);
                    n = spPLC.BytesToRead;
                    LogHelper.Info($"温压流模块PLC串口接收指令长度{n}");

                    overtime++;//超时计数
                    if (overtime >= overTimes)
                    {
                        factorsValues[3] = factorsValues[4] = factorsValues[5] = factorsValues[6] = factorsValues[7] = "0.00";
                        factorsStates[3] = "连接超时";
                        spPLC.Close();//关闭串口
                        return;
                    }
                }
                spPLC.Read(recebuf, 0, 11);//读取缓冲数据 
                LogHelper.Info($"温压流模块PLC串口读取缓冲数据成功");

                int wendu = recebuf[3] << 8 | recebuf[4];
                double yali = recebuf[5] << 8 | recebuf[6];
                double liusu = recebuf[7] << 8 | recebuf[8];
                factorsValues[3] = wendu.ToString();
                factorsValues[4] = (yali / 1000).ToString("#0.0000");
                factorsValues[5] = (liusu / 10).ToString();
                factorsStates[3] = factorsStates[4] = factorsStates[5] = factorsStates[6] = factorsStates[7] = "测量正常";

                //var fixParaSetting = GetFixParaSettingInfo();//读取系统设置参数

                Kv = float.Parse(fixParaSetting.VelocityFieldValue.Value.ToString());//速度场系数
                Vp = float.Parse(factorsValues[5]) * 1000;
                Vs = Kv * Vp;
                F = float.Parse(fixParaSetting.FlueArea.Value.ToString());//烟道面积
                Ba = float.Parse(fixParaSetting.AtmosphericPressure.Value.ToString());//大气压力
                Ps = float.Parse(factorsValues[4]);
                ts = float.Parse(factorsValues[3]);
                Xsw = float.Parse(fixParaSetting.FlueGasHumidity.Value.ToString());//烟气湿度

                spPLC.Close();//关闭串口
                LogHelper.Info($"温压流模块PLC串口关闭成功");

                float Qs = Vs * F * 3600;
                factorsValues[6] = (Qs * (273 / (273 + ts)) * ((Ba + Ps) / 101325) * (1 - (Xsw / 100))).ToString("#0.00");//标况
                factorsValues[7] = Qs.ToString("#0.00");//工况流量
            }
            catch (Exception ex)
            {
                LogHelper.Error($"温压流模块读取出错", ex);
                factorsValues[3] = factorsValues[4] = factorsValues[5] = factorsValues[6] = factorsValues[7] = "0.00";
                factorsStates[3] = "连接超时";

                GlobalVariables.MaxErrorTimes++;
            }
        }

        /// <summary>
        /// 重新加载系统设置参数
        /// </summary>
        /// <returns></returns>
        private void ReloadSettingInfo()
        {
            using (var db = new DatabaseContext())
            {
                fixParaSetting = db.BasSysFixedParaSettings.Where(p => p.Deleted == false).FirstOrDefault();

                //读取设备端口设置信息集合
                equInfos = db.BasFactorEquSettings.Where(p => p.Deleted == false).OrderBy(p => p.Id)
                    .Select(p => new BasFactorEquSettingModel
                    {
                        Id = p.Id,
                        ElectricalRangeBegin = p.ElectricalRangeBegin,
                        ElectricalRangeEnd = p.ElectricalRangeEnd,
                        ModuleChannelIn = p.ModuleChannelIn
                    }).ToList();

                for (int i = 0; i < equInfos.Count; i++)
                {
                    minlc[i] = float.Parse(equInfos[i].ElectricalRangeEnd.ToString());//电气量程终
                    maxlc[i] = float.Parse(equInfos[i].ElectricalRangeBegin.ToString());//电气量程始
                    lc[i] = maxlc[i] - minlc[i];
                    gp[i] = equInfos[i].ModuleChannelIn;//输入模块通道号
                }

                //读取端口号和波特率信息集合
                comSettingInfos = db.BssComSettings.Where(p => p.Deleted == false)
                    .Select(p => new BssComSettingModel
                    {
                        ComTarget = p.ComTarget,
                        ComNumber = p.ComNumber,
                        BaudRate = p.BaudRate,
                        CheckDigit = p.CheckDigit,
                        DataBit = p.DataBit,
                        StopBit = p.StopBit,
                        ComType = p.ComType
                    }).ToList();
            }

            ////重新加载数据之后，重新从ComSetting表中循环初始化各个接口
            //LoadComsFromSetting();
        }

        /// <summary>
        /// 延时函数
        /// </summary>
        /// <param name="mSecond"></param>
        public static void Delay(int mSecond)
        {
            Thread.Sleep(mSecond);
        }
        #endregion

        #region 按钮区命令
        private bool CanExcute() { return true; }

        /// <summary>
        /// 数据清空
        /// </summary>
        private void ClearData()
        {
            if (processing == true) return;
            try
            {
                processing = true;
                //提示是否绑定本机
                if (MessageBox.Show("确定要清除数据？", "系统提示", MessageBoxButton.OKCancel, MessageBoxImage.Information) == MessageBoxResult.OK)
                {
                    using (var db = new DatabaseContext())
                    {
                        db.HistoryMinutes.RemoveRange(db.HistoryMinutes.Where(p => p.Deleted == false).ToList());
                        db.HistoryHours.RemoveRange(db.HistoryHours.Where(p => p.Deleted == false).ToList());
                        db.HistoryDays.RemoveRange(db.HistoryDays.Where(p => p.Deleted == false).ToList());
                        db.HistoryMonths.RemoveRange(db.HistoryMonths.Where(p => p.Deleted == false).ToList());

                        var result = db.SaveChanges() > 0;
                        var msg = result ? "成功" : "失败";
                        MessageBox.Show($"数据清除{msg}!");
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"清除数据出错，错误信息：{ex}");
                LogHelper.Error($"清除数据出错", ex);
            }
            finally
            {
                processing = false;
            }
        }

        /// <summary>
        /// 校准模式
        /// </summary>
        /// <returns></returns>
        private void CalibrateAsync()
        {
            if (processing == true) return;
            try
            {
                processing = true;
                if (caliFlag)
                {
                    CalibrateContent = "退出校准";
                    caliFlag = false;
                    for (int i = 0; i < 20; i++)
                    {
                        factorsStates[i] = "正在校准";
                    }
                }
                else
                {
                    CalibrateContent = "校准模式";
                    caliFlag = true;
                    for (int i = 0; i < 20; i++)
                    {
                        factorsStates[i] = "连接中";
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"标准模式出错，错误信息：{ex}");
                LogHelper.Error($"标准模式出错", ex);
            }
            finally
            {
                processing = false;
            }
        }

        /// <summary>
        /// 参数设置
        /// </summary>
        private void ParaSeting()
        {
            if (processing == true) return;
            try
            {
                processing = true;

                ParaSetting paraSetting = new ParaSetting(this);
                paraSetting.ShowDialog();

                LogHelper.Info($"重新加载设置参数");
                ReloadSettingInfo();
                //MessageBox.Show($"{testNumber}");//测试数据传输
            }
            catch (Exception ex)
            {
                MessageBox.Show($"打开参数设置窗口错误，错误信息：{ex}");
                LogHelper.Error($"打开参数设置窗口错误", ex);
            }
            finally
            {
                processing = false;
            }
        }


        #endregion

        #region 辅助方法
        #region Search异步方法
        void bw_DoWork(object sender, System.ComponentModel.DoWorkEventArgs e)
        {
            try
            {
                HomePageProjectList.Where(p => p.FactorName == "总烃").FirstOrDefault().FactorValue = DateTime.Now.ToString("yyyy-MM-dd HH:mm:ss");//总烃值
                HomePageProjectList.Where(p => p.FactorName == "非甲烷总烃").FirstOrDefault().FactorValue = DateTime.Now.AddDays(1).ToString("yyyy-MM-dd HH:mm:ss");//非甲烷总烃值
            }
            catch (Exception ex)
            {
                MessageBox.Show($"测试数据错误，错误信息：{ex}");

                bw.CancelAsync();
            }
            if (bw.CancellationPending)
            {
                e.Cancel = true;
            }
            else
            {
                e.Cancel = false;
            }
        }

        void bw_RunWorkerCompleted(object sender, System.ComponentModel.RunWorkerCompletedEventArgs ce)
        {
            if (ce.Cancelled)
            {
                MessageBox.Show("读取失败，请重试！");
            }
            else
            {

            }
            processing = false;
        }
        #endregion

        /// <summary>
        /// 开启定时调度计划
        /// </summary>
        /// <returns></returns>
        private async Task QuartzLoadInfo()
        {
            //通过调度工厂获得调度器
            _scheduler = await factory.GetScheduler();
            //开启调度器
            await _scheduler.Start();
            //创建触发器1
            var trigger1S = TriggerBuilder.Create()
                            .WithSimpleSchedule(x => x.WithIntervalInSeconds(5).RepeatForever())//每1秒执行一次
                            .Build();
            //创建任务
            var jobLoadFactorInfoSecond = JobBuilder.Create<LoadFactorInfoSecondJob>()
                            .WithIdentity("job", "group")
                            .Build();
            //将触发器和任务器绑定到调度器中
            await _scheduler.ScheduleJob(jobLoadFactorInfoSecond, trigger1S);


            ////创建触发器2
            //var trigger3S = TriggerBuilder.Create()
            //                .WithSimpleSchedule(x => x.WithIntervalInSeconds(3).RepeatForever())//每1秒执行一次
            //                .Build();
            ////创建任务
            //var jobLoadFactorInfoSecond = JobBuilder.Create<LoadFactorInfoSecondJob>()
            //                .WithIdentity("job", "group")
            //                .Build();
            ////将触发器和任务器绑定到调度器中
            //await _scheduler.ScheduleJob(jobLoadFactorInfoSecond, trigger3S);
        }

        /// <summary>
        /// 因子折算处理
        /// </summary>
        /// <param name="result"></param>
        private void FactorDataHandle(List<BssFactorModel> result)
        {
            //非甲烷总烃 = 总烃 - 甲烷（值为负的话显示0值）
            var nmhcValue = result.Where(p => p.FactorName == "总烃").Select(p => decimal.Parse(p.FactorValue)).FirstOrDefault()
              - result.Where(p => p.FactorName == "甲烷").Select(p => decimal.Parse(p.FactorValue)).FirstOrDefault();

            if (result.Any(p => p.FactorName == "非甲烷总烃"))
            {
                result.Where(p => p.FactorName == "非甲烷总烃").FirstOrDefault().FactorValue = nmhcValue < 0 ? "0.00" : nmhcValue.ToString();
            }
        }

        /// <summary>
        /// 根据状态码返回状态
        /// </summary>
        /// <param name="projectState"></param>
        /// <returns></returns>
        private string GetProjectState(int projectState)
        {
            var result = string.Empty;
            switch (projectState)
            {
                case 0:
                    result = "连接中";
                    break;
                case 1:
                    result = "测量正常";
                    break;
                default:
                    result = "连接超时";
                    break;
            }

            return result;
        }
        #endregion

        #region 4个指令按钮
        /// <summary>
        /// FID-A
        /// </summary>
        private void FIDA()
        {
            if (processing == true) return;
            try
            {
                processing = true;

                ToolsHepler tools = new ToolsHepler();//数据获取帮助类
                var comInfo = comSettingInfos.Where(p => p.ComTarget == "ModBus").FirstOrDefault();
                SerialPort spCom = new SerialPort(comInfo.ComNumber, comInfo.BaudRate, Parity.None, comInfo.DataBit ?? 8);

                TbFIDA = tools.Command_NBGC60(10, spCom, 3);
            }
            catch (Exception ex)
            {
                MessageBox.Show($"FID-A出错，错误信息：{ex}");
                LogHelper.Error($"FID-A出错", ex);
            }
            finally
            {
                processing = false;
            }
        }

        /// <summary>
        /// FID-B
        /// </summary>
        private void FIDB()
        {
            if (processing == true) return;
            try
            {
                processing = true;

                ToolsHepler tools = new ToolsHepler();//数据获取帮助类
                var comInfo = comSettingInfos.Where(p => p.ComTarget == "ModBus").FirstOrDefault();
                SerialPort spCom = new SerialPort(comInfo.ComNumber, comInfo.BaudRate, Parity.None, comInfo.DataBit ?? 8);

                TbFIDB = tools.Command_NBGC60(11, spCom, 3);
            }
            catch (Exception ex)
            {
                MessageBox.Show($"FID-B出错，错误信息：{ex}");
                LogHelper.Error($"FID-B出错", ex);
            }
            finally
            {
                processing = false;
            }
        }

        /// <summary>
        /// 获取色谱柱实时温度
        /// </summary>
        private void ChromatographicColumn()
        {
            if (processing == true) return;
            try
            {
                processing = true;

                ToolsHepler tools = new ToolsHepler();//数据获取帮助类
                var comInfo = comSettingInfos.Where(p => p.ComTarget == "ModBus").FirstOrDefault();
                SerialPort spCom = new SerialPort(comInfo.ComNumber, comInfo.BaudRate, Parity.None, comInfo.DataBit ?? 8);

                TbChromatographicColumn = tools.Command_NBGC60(12, spCom, 3);
            }
            catch (Exception ex)
            {
                MessageBox.Show($"获取色谱柱实时温度出错，错误信息：{ex}");
                LogHelper.Error($"获取色谱柱实时温度出错", ex);
            }
            finally
            {
                processing = false;
            }
        }

        /// <summary>
        /// 获取已完成次数及仪器状态
        /// </summary>
        private void CompleteInstrumentStatus()
        {
            if (processing == true) return;
            try
            {
                processing = true;

                ToolsHepler tools = new ToolsHepler();//数据获取帮助类
                var comInfo = comSettingInfos.Where(p => p.ComTarget == "ModBus").FirstOrDefault();
                SerialPort spCom = new SerialPort(comInfo.ComNumber, comInfo.BaudRate, Parity.None, comInfo.DataBit ?? 8);

                var result = tools.Command_NBGC60(13, spCom, 3);
                var convertResult = result.Split('.').FirstOrDefault().ToString();
                convertResult = convertResult.Substring(convertResult.Length - 1, 1);

                switch (convertResult)
                {
                    case "0":
                        TbCompleteInstrumentStatus = "开机运行";
                        break;
                    case "1":
                        TbCompleteInstrumentStatus = "异常熄火";
                        break;
                    case "2":
                        TbCompleteInstrumentStatus = "温度异常";
                        break;
                    case "3":
                        TbCompleteInstrumentStatus = "仪器稳定";
                        break;
                    case "5":
                        TbCompleteInstrumentStatus = "正常分析";
                        break;
                    case "6":
                        TbCompleteInstrumentStatus = "点火失败";
                        break;
                    default:
                        TbCompleteInstrumentStatus = result.ToString();
                        break;
                }

            }
            catch (Exception ex)
            {
                MessageBox.Show($"获取已完成次数及仪器状态出错，错误信息：{ex}");
                LogHelper.Error($"获取已完成次数及仪器状态出错", ex);
            }
            finally
            {
                processing = false;
            }
        }
        #endregion

        #endregion
    }
}
