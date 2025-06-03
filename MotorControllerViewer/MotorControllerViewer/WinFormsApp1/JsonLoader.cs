

using Newtonsoft.Json;
using System.Reflection;

namespace WinFormsApp1
{
    public class JsonLoader
    {

        public static void Save<T>(T instance)
        {
            var serei = JsonConvert.SerializeObject(instance, Formatting.Indented);

            var type = typeof(T);
            var cont = Path + "/" + CSharpName(type) + ".json";
            File.WriteAllText(cont, serei);
        }

        public static T Load<T>()
        {
            var type = typeof(T);
            var cont = Path + "/" + CSharpName(type) + ".json";

            if(!File.Exists(cont))
            {
                var newt = Activator.CreateInstance<T>();
                Save(newt);
                return newt;
            }

            var json = File.ReadAllText(cont);
            var obj = JsonConvert.DeserializeObject<T>(json);
            return obj;
        }

        public static string CSharpName(Type type)
        {
            string typeName = type.Name;

            if (type.IsGenericType)
            {
                var genArgs = type.GetGenericArguments();

                if (genArgs.Count() > 0)
                {
                    typeName = typeName.Substring(0, typeName.Length - 2);
                    string args = "";

                    foreach (var argType in genArgs)
                    {
                        string argName = argType.Name;
                        if (argType.IsGenericType) argName = CSharpName(argType);
                        args += argName + ", ";
                    }

                    typeName = string.Format("{0}[{1}]", typeName, args.Substring(0, args.Length - 2));
                }
            }

            return typeName;
        }

        private static string Path;

        static JsonLoader()
        {
            Assembly ass = Assembly.GetExecutingAssembly();
            Path = System.IO.Path.GetDirectoryName(ass.ManifestModule.FullyQualifiedName).Replace("\\", "/");
        }
    }
}
