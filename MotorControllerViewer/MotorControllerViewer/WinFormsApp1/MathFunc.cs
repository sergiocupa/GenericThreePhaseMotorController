
namespace ProfitCapture.Utils
{
    public class MathFunc
    {

        internal static decimal Integrate(decimal valorAtual, decimal valorAnterior, decimal Q, decimal samble_interval)
        {
            if (samble_interval == 0) samble_interval = 1;
            var sampling_rate = 1.0m / samble_interval;
            var tau = Q * sampling_rate;

            decimal _int = (valorAtual + (valorAnterior * tau)) / (tau + 1.0m);
            return _int;
        }

        internal static float Integrate(float valorAtual, float valorAnterior, float Q, float samble_interval)
        {
            if (samble_interval == 0) samble_interval = 1;
            var sampling_rate = 1.0f / samble_interval;
            var tau = Q * sampling_rate;

            float _int = (valorAtual + (valorAnterior * tau)) / (tau + 1.0f);
            return _int;
        }

    }
}
