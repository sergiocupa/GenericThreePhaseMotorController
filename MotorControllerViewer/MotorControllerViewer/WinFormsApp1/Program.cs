using System.Globalization;

namespace WinFormsApp1
{

    internal static class Program
    {
        [STAThread]
        static void Main()
        {
            CultureInfo.DefaultThreadCurrentCulture = CultureInfo.InvariantCulture; // Define a cultura padr�o para o aplicativo

            //Application.ThreadException += new ThreadExceptionEventHandler(FormPrincipal_UIThreadException);  
            //Application.SetUnhandledExceptionMode(UnhandledExceptionMode.CatchException);  
            //AppDomain.CurrentDomain.UnhandledException += new UnhandledExceptionEventHandler(CurrentDomain_UnhandledException);  

            Application.EnableVisualStyles(); // Substitu�do para corrigir o erro  
            Application.SetCompatibleTextRenderingDefault(false); // Substitu�do para corrigir o erro  

            Application.Run(new Form1());
        }
    }
}