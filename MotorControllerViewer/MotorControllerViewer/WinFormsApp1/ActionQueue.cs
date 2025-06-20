﻿
using System;

namespace WinFormsApp1
{
    internal class ActionQueue<TData>
    {
        private void Running()
        {
            Rodando = true;
            while (Rodando)
            {
                Aguarde.Wait();

                var a = new ClientEventItem[0];
                lock (Lock)
                {
                    a = Fila.ToArray();
                    Fila.Clear();
                    Aguarde.Reset();
                }

                int ix = 0;
                while (ix < a.Length)
                {
                    a[ix].Method(a[ix].Data);
                    ix++;
                }
            }
        }


        public void Enqueue(Action<TData> method, TData data)
        {
            ClientEventItem action = new ClientEventItem() { Method = method, Data = data };

            lock (Lock)
            {
                Fila.Add(action);
                Aguarde.Set();
            }
        }


        private void Start()
        {
            if (!Rodando)
            {
                Aguarde = new ManualResetEventSlim();
                Aguarde.Reset();

                thr = new Thread(Running);
                thr.Start();
            }
        }
        public void Stop()
        {
            Rodando = false;
            if (Aguarde != null) Aguarde.Set();
        }

        public int Count()
        {
            var cnt = 0;
            lock (Lock) { cnt = Fila.Count; }
            return cnt;
        }


        private ClientEventItem Lock;
        private Thread thr;
        private ManualResetEventSlim Aguarde;
        private List<ClientEventItem> Fila;
        private bool Rodando;


        public ActionQueue()
        {
            Fila = new List<ClientEventItem>();
            Rodando = false;
            Lock = new ClientEventItem();
            Start();
        }


        class ClientEventItem
        {
            public Action<TData> Method { get; set; }
            public TData Data { get; set; }
            public bool Important { get; set; }
        }

    }
}
