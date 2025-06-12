using CapturaTestesGUI.Utils;
using System.IO.Ports;


namespace WinFormsApp1
{

    public partial class Form1 : Form
    {

        LogFrame Prev = new LogFrame();
        bool First = true;

        private void Received(byte[] data)
        {
            Counter++;

            var values = LogFrameParser.Parse(data);

            if (values.Count > 0)
            {
                CountData += values.Count;

                var first = values.FirstOrDefault();

                label3.Invoke(() => 
                { 
                    label3.Text = "Rec: " + Counter + " | Total: " + CountData + " | Freq.: " + first.Frequency + " | Tick: " + first.TicksByIteration + " | Jumps: " + BufferJumps;
                    label3.Refresh();
                });

                var SerieR = Chart.GetSerie(2);
                var SerieS = Chart.GetSerie(3);
                var SerieT = Chart.GetSerie(4);

                if (First)
                {
                    Prev.Counter = first.Counter -1;
                    First = false;
                }

                int dif = 0;
                int ix = 0;
                while (ix < values.Count)
                {
                    LogFrame line = values[ix];

                    Chart.CreatePoint(SerieR, Motion, line.WaveR);
                    Chart.CreatePoint(SerieS, Motion, line.WaveS);
                    Chart.CreatePoint(SerieT, Motion, line.WaveT);

                    Motion      += line.MotionStep;
                    BufferJumps += (line.Counter - (Prev.Counter + 1));

                    Prev = line;
                    ix++;
                }
            }
        }

        private void button1_Click(object sender, EventArgs e)
        {
            try
            {
                if (Serial == null)
                {
                    button1.Enabled = false;

                    Serial = new SerialPort("COM4", 9600, Parity.None, 8, StopBits.One);

                    Serial.ReadBufferSize  = 128000;
                    Serial.WriteBufferSize = 128000;

                    Serial.DataReceived += (object sender, SerialDataReceivedEventArgs e) =>
                    {
                        var bytes = new byte[Serial.BytesToRead];
                        Serial.Read(bytes, 0, bytes.Length);
                        AQueue.Enqueue(Received, bytes);
                    };

                    Serial.Open();
                    button1.Enabled = false;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
        }

        private void ArquivoButton_Click(object sender, EventArgs e)
        {
            var ofd = new OpenFileDialog();
            var ok = ofd.ShowDialog();
            if (ok == DialogResult.OK)
            {
                FileTextBox.Text = ofd.FileName;
            }
        }


        private void button2_Click(object sender, EventArgs e)
        {
            try
            {
                AddressTitles = JsonLoader.Load<List<AdrressInfo>>();

                var content = string.Join("\n", AddressTitles.Select(s => s.Address + "=" + s.Description));

                var te = new TitleEdit();
                te.Content.Text = content;
                te.ShowDialog(this);

                content = te.Content.Text;

                var lines = content.Split(new[] { "\n" }, StringSplitOptions.RemoveEmptyEntries);

                AddressTitles.Clear();

                foreach (var line in lines)
                {
                    var parts = line.Split('=');
                    if (parts.Length == 2)
                    {
                        var addressInfo = new AdrressInfo()
                        {
                            Address = int.Parse(parts[0]),
                            Description = parts[1]
                        };
                        AddressTitles.Add(addressInfo);
                    }
                }

                JsonLoader.Save(AddressTitles);

                RenderTitles();
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
        }


        public void RenderTitles()
        {
            var act = () =>
            {
                try
                {
                    panel3.Controls.Clear();

                    int ix = AddressTitles.Count;
                    while (ix > 0)
                    {
                        ix--;
                        var exist = AddressTitles[ix];

                        exist.ForeColor = ColorGenerator.Create();
                        exist.Label = new Label()
                        {
                            Font = new Font("Arial", 10, FontStyle.Bold),
                            Text = exist.Description,
                            Dock = DockStyle.Left,
                            AutoSize = true,
                            ForeColor = exist.ForeColor
                        };
                        panel3.Controls.Add(exist.Label);

                        Chart.CreateSerie(exist.Address, SerieLineType.Line, exist.ForeColor);
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.ToString());
                }
            };

            if (panel3.InvokeRequired)
            {
                panel3.Invoke(act);
            }
            else
            {
                act();
            }
        }


        public void Run()
        {
            try
            {
                while (Running)
                {
                    Pheta += 0.01;

                    var X = 10 * Math.Cos(Pheta);
                    var Y = 10 * Math.Sin(Pheta);

                    Chart.CreatePoint("R", Pheta, X);
                    Chart.CreatePoint("S", Pheta, Y);

                    //Chart.CreatePoint("R", 0, 5);
                    // Chart.CreatePoint("R", 12, 8);
                    //Chart.CreatePoint("R", 12, 5);

                    //Chart.CreatePoint("R", 0, 5);
                    //Chart.CreatePoint("R", 5, 10);
                    //Chart.CreatePoint("R", 6, 8);
                    //Chart.CreatePoint("R", 12, 2);
                    //Chart.CreatePoint("R", 6, -1);

                    //Chart.CreatePoint("S", 4, 4);
                    // Chart.CreatePoint("S", 6, 3);
                    //Chart.CreatePoint("S", 3, 6);
                    //Chart.CreatePoint("S", 5, 1);
                    //Chart.CreatePoint("S", 4, -2);

                    //Chart.CreatePoint("R", 0, 5);
                    //Chart.CreatePoint("R", 5, -5);

                    //Chart.CreatePoint("R", 5, 0);
                    //Chart.CreatePoint("R", 10, 5);

                    //Chart.Append(Pheta, Y, "1");
                    //Thread.Sleep(1);
                    //break;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
            }
            finally
            {
                button1.Invoke(() => { button1.Enabled = true; });
                Thr = null;
            }
        }


        private void Form1_FormClosing(object? sender, FormClosingEventArgs e)
        {
            try
            {
                if(Serial != null)
                {
                    Serial.Close();
                }

                AQueue.Stop();
            }
            catch (Exception ex)
            {

                throw;
            }
        }



        private double Pheta;
        private ChartControl Chart;
        private Thread Thr;
        private bool Running;
        private SerialPort Serial;
        private ActionQueue<byte[]> AQueue;
        private long Counter;
        private long CountData;
        private double Motion;
        List<AdrressInfo> AddressTitles;
        private long BufferJumps;
        //private long BufferCounter;

        public Form1()
        {
            InitializeComponent();

            FormClosing += Form1_FormClosing;

            Counter = 0;

            AQueue = new ActionQueue<byte[]>();

            Chart = new ChartControl() { Dock = DockStyle.Fill };
            panel2.Controls.Add(Chart);

            AddressTitles = JsonLoader.Load<List<AdrressInfo>>();
            RenderTitles();
        }

        
    }
}
