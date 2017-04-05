defmodule Complex do

  def foo(x), do:
    call_cnode {:foo, x}
  def bar(y), do:
    call_cnode {:bar, y}

  defp call_cnode(msg) do
    {:ok, hostname} = :inet.gethostname()
    send {:any, String.to_atom("c1@#{hostname}") }, {:call, self, msg}
    receive do
      {:cnode, result} -> result
    end
  end
end
