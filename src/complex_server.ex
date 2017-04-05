defmodule Complex do
  use GenServer

  # client API
  def start_link() do
    {:ok, pid} = GenServer.start_link(__MODULE__, :ok, [])
    pid
  end

  def foo( pid, n ) do
    GenServer.cast(pid, :foo, n)
  end

  # callbacks
  def handle_cast( :foo, _from, x) do
    { :noreply, call_cnode( {:foo, x} ), {} }
  end

  def handle_info( {:cnode, result}, _state) do
    { :nroeply, result }
  end

  defp call_cnode(msg) do
    { :ok, hostname } = :inet.gethostname()
    send { :any, String.to_atom("c1@#{hostname}") }, { :call, self, msg }
  end
end
