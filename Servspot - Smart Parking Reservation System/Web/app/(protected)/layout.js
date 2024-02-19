import DrawerAppBar from "../component/DrawerAppBar";

export default function Layout({ children }) {
  return (
    <>
      <DrawerAppBar />
      {children}
    </>
  );
}
