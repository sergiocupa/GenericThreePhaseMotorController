using System.Globalization;

namespace WinFormsApp1
{

    internal static class Program
    {
        [STAThread]
        static void Main()
        {
            CultureInfo.DefaultThreadCurrentCulture = CultureInfo.InvariantCulture; // Define a cultura padrão para o aplicativo

            //Application.ThreadException += new ThreadExceptionEventHandler(FormPrincipal_UIThreadException);  
            //Application.SetUnhandledExceptionMode(UnhandledExceptionMode.CatchException);  
            //AppDomain.CurrentDomain.UnhandledException += new UnhandledExceptionEventHandler(CurrentDomain_UnhandledException);  

            Application.EnableVisualStyles(); // Substituído para corrigir o erro  
            Application.SetCompatibleTextRenderingDefault(false); // Substituído para corrigir o erro  

            Application.Run(new Form1());
        }
    }
}