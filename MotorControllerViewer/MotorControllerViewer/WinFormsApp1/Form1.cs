namespace WinFormsApp1
{
    public partial class Form1 : Form
    {


        public void Run()
        {
            try
            {
                while(Running)
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

        double Pheta;
        ChartControl Chart;
        Thread Thr;
        bool Running;

        public Form1()
        {
            InitializeComponent();

            Chart = new ChartControl() { Dock = DockStyle.Fill };

            Chart.CreateSerie("R");
            Chart.CreateSerie("S");
            Chart.CreateSerie("T");


            panel2.Controls.Add(Chart);
        }

        private void button1_Click(object sender, EventArgs e)
        {
            button1.Enabled = false;
            Running = true;
            Thr = new Thread(Run);
            Thr.Start();
        }
    }
}
