import { Box, Flex } from "@chakra-ui/react";
import type { ReactNode } from "react";

import Meta from "./Meta";
import Nav from "./Nav"
type LayoutProps = {
  children: ReactNode;
};

const Layout = ({ children }: LayoutProps) => {

  return (
    <Box margin="0 auto" transition="0.5s ease-out">
      <Nav />
      <Meta />
      <Flex wrap="wrap" margin="8" minHeight="90vh">
        <Box width="full" as="main" marginY={22}>
          {children}
        </Box>
      </Flex>
    </Box>
  );
};

export default Layout;
